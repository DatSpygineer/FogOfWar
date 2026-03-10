#include "PropertyEditor.hpp"

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>

#include <fow/Shared.hpp>
#include <fow/Renderer.hpp>

#include "AssetFileTree.hpp"
#include "util/QtHelpers.hpp"

namespace fow {
    static const char* attribute_value_or(const pugi::xml_attribute& attribute, const char* _default) {
        return attribute ? attribute.value() : _default;
    }

    Result<DataMapping> DataMapping::Load(const String& name) {
        const auto result = LoadEditorMappingXml(name);
        if (result.has_value()) {
            return FromXml(name, result.value().child("Mapping"));
        }
        return Failure(result.error());
    }

    Result<DataMapping> DataMapping::FromXml(const String& name, const pugi::xml_node& node) {
        const auto class_name_node = node.child("ClassName");
        if (!class_name_node) {
            return Failure(std::format("Failed to load mapping \"{}\": Expected node \"ClassName\"", name));
        }
        const auto params_node = node.child("Params");
        if (!params_node) {
            return Failure(std::format("Failed to load mapping \"{}\": Expected node \"Params\"", name));
        }

        DataMapping mapping(class_name_node.child_value());
        for (const auto& param : params_node.children()) {
            DataMappingEntry entry;
            const auto param_name_attrib = param.attribute("name");
            if (!param_name_attrib) {
                return Failure(std::format("Failed to load mapping \"{}\": Param node must have 'name' attribute!", name));
            }
            const auto param_type_attrib = param.attribute("type");
            if (!param_type_attrib) {
                return Failure(std::format("Failed to load mapping \"{}\": Param node must have 'type' attribute!", name));
            }
            const auto display_name_node = param.attribute("displayAs");
            if (display_name_node) {
                entry.displayName = display_name_node.value();
            }

            entry.name = param_name_attrib.value();
            const auto param_type_names = String(param_type_attrib.value()).split(':');
            auto type = Match<String, DataMappingType>(param_type_names.at(0).clone_lowercase(), HashMap<String, DataMappingType> {
                { "bool"_s,          DataMappingType::Bool         },
                { "int"_s,           DataMappingType::Int          },
                { "float"_s,         DataMappingType::Float        },
                { "string"_s,        DataMappingType::String       },
                { "choices"_s,       DataMappingType::Choices      },
                { "vector2"_s,       DataMappingType::Vector2      },
                { "vector3"_s,       DataMappingType::Vector3      },
                { "vector4"_s,       DataMappingType::Vector4      },
                { "color"_s,         DataMappingType::Color        },
                { "array"_s,         DataMappingType::Array        },
                { "map"_s,           DataMappingType::Map          },
                { "asset"_s,         DataMappingType::Asset        },
                { "shadername"_s,    DataMappingType::ShaderName   },
                { "shader_name"_s,   DataMappingType::ShaderName   }
            });
            if (!type.has_value()) {
                return Failure(std::format("Failed to load mapping \"{}\": Param node 'type' has invalid type name \"{}\"", name, param_type_attrib.value()));
            }
            entry.type = type.value();

            auto is_asset = entry.type == DataMappingType::Asset;
            if (!is_asset && (entry.type == DataMappingType::Array || entry.type == DataMappingType::Map)) {
                if (param_type_names.size() < 2) {
                    return Failure(std::format("Failed to load mapping \"{}\": Param node 'type' has invalid type name \"{}\", expected item type!", name, param_type_attrib.value()));
                }

                auto item_type = Match<String, DataMappingType>(param_type_names.at(1).clone_lowercase(), HashMap<String, DataMappingType> {
                    { "bool"_s,          DataMappingType::Bool         },
                    { "int"_s,           DataMappingType::Int          },
                    { "float"_s,         DataMappingType::Float        },
                    { "string"_s,        DataMappingType::String       },
                    { "choices"_s,       DataMappingType::Choices      },
                    { "vector2"_s,       DataMappingType::Vector2      },
                    { "vector3"_s,       DataMappingType::Vector3      },
                    { "vector4"_s,       DataMappingType::Vector4      },
                    { "color"_s,         DataMappingType::Color        },
                    { "asset"_s,         DataMappingType::Asset        }
                });
                if (!item_type.has_value()) {
                    return Failure(std::format("Failed to load mapping \"{}\": Param node 'type' has invalid type name \"{}\"", name, param_type_attrib.value()));
                }
                entry.itemType = item_type.value();
                is_asset = item_type.value() == DataMappingType::Asset;
            }

            if (is_asset) {
                entry.assetClassName = param_type_names.back();
            }

            entry.category      = attribute_value_or(param.attribute("category"), "");
            entry.optional      = StringToBool(attribute_value_or(param.attribute("optional"), "true")).value_or(true);
            entry.isAttribute   = StringToBool(attribute_value_or(param.attribute("is_attribute"), "false")).value_or(false);
            entry.arrayItemName = attribute_value_or(param.attribute("arrayItemName"), "Item");

            const auto default_node = param.child("Default");
            entry.defaultValue = default_node ? default_node.child_value() : "";
            const auto range_node = param.child("Range");
            if (range_node) {
                entry.minValue = attribute_value_or(range_node.attribute("min"), "");
                entry.maxValue = attribute_value_or(range_node.attribute("max"), "");
            }

            entry.keys   = Vector<String> { };
            entry.values = Vector<String> { };
            const auto choices_node = param.child("Values");
            for (const auto& value : choices_node.children()) {
                entry.keys.emplace_back(value.name());
                entry.values.emplace_back(value.child_value());
            }
            mapping.m_entryOrder.emplace_back(param_name_attrib.value());
            mapping.m_entries.emplace(param_name_attrib.value(), std::move(entry));
        }
        return Success(std::move(mapping));
    }

    PropertyEditorEntry::PropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent)
        : QWidget(parent), m_sName(name), m_mapping(mapping_entry), m_pBaseLayout(new QHBoxLayout) {
        setLayout(m_pBaseLayout);
    }

    void PropertyEditorEntry::createLabel() {
        m_pBaseLayout->addWidget(std::move(new QLabel(m_sName.as_cstr())), 1);
    }
    void PropertyEditorEntry::setup() {
        QtHelpers::ClearLayout(m_pBaseLayout);
        createLabel();
        if (createEntry(m_pBaseLayout)) {
            m_bEntryCreated = true;
        }
    }

    BooleanPropertyEditorEntry::BooleanPropertyEditorEntry(const String& name, const bool defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pCheckBox(new QCheckBox) {
        m_pCheckBox->setChecked(defaultValue);
    }

    bool BooleanPropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pCheckBox == nullptr) {
                m_pCheckBox = new QCheckBox;
            }
            layout->addWidget(m_pCheckBox, 4);
            connect(m_pCheckBox, &QCheckBox::checkStateChanged, this, &BooleanPropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    bool BooleanPropertyEditorEntry::getValue() const {
        return m_pCheckBox->isChecked();
    }
    void BooleanPropertyEditorEntry::setValue(const bool value) const {
        m_pCheckBox->setChecked(value);
    }

    IntPropertyEditorEntry::IntPropertyEditorEntry(const String& name, const int defaultValue, const int min, const int max, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pSpinBox(new QSpinBox) {
        m_pSpinBox->setMinimum(min);
        m_pSpinBox->setMaximum(max);
        m_pSpinBox->setValue(defaultValue);
    }

    bool IntPropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pSpinBox == nullptr) {
                m_pSpinBox = new QSpinBox;
            }
            layout->addWidget(m_pSpinBox, 4);
            connect(m_pSpinBox, &QSpinBox::valueChanged, this, &IntPropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    int IntPropertyEditorEntry::getValue() const {
        return m_pSpinBox->value();
    }
    void IntPropertyEditorEntry::setValue(const int value) const {
        m_pSpinBox->setValue(value);
    }

    void IntPropertyEditorEntry::setMinValue(const int min) const {
        m_pSpinBox->setMinimum(min);
    }
    int IntPropertyEditorEntry::getMinValue() const {
        return m_pSpinBox->minimum();
    }

    void IntPropertyEditorEntry::setMaxValue(const int max) const {
        m_pSpinBox->setMaximum(max);
    }
    int IntPropertyEditorEntry::getMaxValue() const {
        return m_pSpinBox->maximum();
    }

    FloatPropertyEditorEntry::FloatPropertyEditorEntry(const String& name, const double defaultValue, const double min, const double max, const double step, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pSpinBox(new QDoubleSpinBox) {
        m_pSpinBox->setMinimum(min);
        m_pSpinBox->setMaximum(max);
        m_pSpinBox->setSingleStep(step);
        m_pSpinBox->setValue(defaultValue);
    }

    bool FloatPropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (m_bEntryCreated) {
            if (m_pSpinBox == nullptr) {
                m_pSpinBox = new QDoubleSpinBox;
            }
            layout->addWidget(m_pSpinBox, 4);
            connect(m_pSpinBox, &QDoubleSpinBox::valueChanged, this, &FloatPropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    double FloatPropertyEditorEntry::getValue() const {
        return m_pSpinBox->value();
    }
    void FloatPropertyEditorEntry::setValue(const double value) const {
        m_pSpinBox->setValue(value);
    }

    void FloatPropertyEditorEntry::setMinValue(const double min) const {
        m_pSpinBox->setMinimum(min);
    }
    double FloatPropertyEditorEntry::getMinValue() const {
        return m_pSpinBox->minimum();
    }

    void FloatPropertyEditorEntry::setMaxValue(const double max) const {
        m_pSpinBox->setMaximum(max);
    }
    double FloatPropertyEditorEntry::getMaxValue() const {
        return m_pSpinBox->maximum();
    }

    StringPropertyEditorEntry::StringPropertyEditorEntry(const String& name, const String& defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pLineEdit(new QLineEdit) {
        m_pLineEdit->setText(defaultValue.as_cstr());
    }

    bool StringPropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pLineEdit == nullptr) {
                m_pLineEdit = new QLineEdit;
            }
            layout->addWidget(m_pLineEdit, 4);
            connect(m_pLineEdit, &QLineEdit::textChanged, this, &StringPropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    String StringPropertyEditorEntry::getValue() const {
        return m_pLineEdit->text().toStdString();
    }
    void StringPropertyEditorEntry::setValue(const String& value) const {
        m_pLineEdit->setText(value.as_cstr());
    }

    ChoicesPropertyEditorEntry::ChoicesPropertyEditorEntry(const String& name, const String& defaultValue, const Vector<String>& choices, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pComboBox(new QComboBox) {
        for (const auto& choice : choices) {
            m_pComboBox->addItem(choice.as_cstr());
        }
        m_pComboBox->setCurrentText(defaultValue.as_cstr());
    }

    bool ChoicesPropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pComboBox == nullptr) {
                m_pComboBox = new QComboBox;
            }
            layout->addWidget(m_pComboBox, 4);
            connect(m_pComboBox, &QComboBox::currentTextChanged, this, &ChoicesPropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    void ChoicesPropertyEditorEntry::setChoices(const Vector<String>& choices) const {
        m_pComboBox->clear();
        for (const auto& choice : choices) {
            m_pComboBox->addItem(choice.as_cstr());
        }
    }
    void ChoicesPropertyEditorEntry::setValue(const String& value) const {
        m_pComboBox->setCurrentText(value.as_cstr());
    }
    String ChoicesPropertyEditorEntry::getValue() const {
        return m_pComboBox->currentText().toStdString();
    }

    Vector2PropertyEditorEntry::Vector2PropertyEditorEntry(const String& name, const Vector2& defaultValue, const Vector2& min, const Vector2& max, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pSpinBoxX(new QDoubleSpinBox), m_pSpinBoxY(new QDoubleSpinBox) {
        m_pSpinBoxX->setValue(defaultValue.x);
        m_pSpinBoxX->setMinimum(min.x);
        m_pSpinBoxX->setMaximum(max.x);
        m_pSpinBoxY->setValue(defaultValue.y);
        m_pSpinBoxY->setMinimum(min.y);
        m_pSpinBoxY->setMaximum(max.y);
    }

    bool Vector2PropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pSpinBoxX == nullptr) {
                m_pSpinBoxX = new QDoubleSpinBox;
            }
            if (m_pSpinBoxY == nullptr) {
                m_pSpinBoxY = new QDoubleSpinBox;
            }
            layout->addWidget(m_pSpinBoxX, 2);
            layout->addWidget(m_pSpinBoxY, 2);
            connect(m_pSpinBoxX, &QDoubleSpinBox::valueChanged, this, &Vector2PropertyEditorEntry::onEditorValueChanged);
            connect(m_pSpinBoxY, &QDoubleSpinBox::valueChanged, this, &Vector2PropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    void Vector2PropertyEditorEntry::setValue(const Vector2& value) const {
        m_pSpinBoxX->setValue(value.x);
        m_pSpinBoxY->setValue(value.y);
    }
    Vector2 Vector2PropertyEditorEntry::getValue() const {
        return Vector2(m_pSpinBoxX->value(), m_pSpinBoxY->value());
    }

    void Vector2PropertyEditorEntry::setMinValue(const Vector2& min) const {
        m_pSpinBoxX->setMinimum(min.x);
        m_pSpinBoxY->setMinimum(min.y);
    }
    Vector2 Vector2PropertyEditorEntry::getMinValue() const {
        return Vector2(m_pSpinBoxX->minimum(), m_pSpinBoxY->minimum());
    }

    void Vector2PropertyEditorEntry::setMaxValue(const Vector2& max) const {
        m_pSpinBoxX->setMaximum(max.x);
        m_pSpinBoxY->setMaximum(max.y);
    }
    Vector2 Vector2PropertyEditorEntry::getMaxValue() const {
        return Vector2(m_pSpinBoxX->maximum(), m_pSpinBoxY->maximum());
    }

    Vector3PropertyEditorEntry::Vector3PropertyEditorEntry(const String& name, const Vector3& defaultValue, const Vector3& min, const Vector3& max, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pSpinBoxX(new QDoubleSpinBox), m_pSpinBoxY(new QDoubleSpinBox), m_pSpinBoxZ(new QDoubleSpinBox) {
        m_pSpinBoxX->setValue(defaultValue.x);
        m_pSpinBoxX->setMinimum(min.x);
        m_pSpinBoxX->setMaximum(max.x);
        m_pSpinBoxY->setValue(defaultValue.y);
        m_pSpinBoxY->setMinimum(min.y);
        m_pSpinBoxY->setMaximum(max.y);
        m_pSpinBoxZ->setValue(defaultValue.z);
        m_pSpinBoxZ->setMinimum(min.z);
        m_pSpinBoxZ->setMaximum(max.z);
    }

    bool Vector3PropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pSpinBoxX == nullptr) {
                m_pSpinBoxX = new QDoubleSpinBox;
            }
            if (m_pSpinBoxY == nullptr) {
                m_pSpinBoxY = new QDoubleSpinBox;
            }
            if (m_pSpinBoxZ == nullptr) {
                m_pSpinBoxZ = new QDoubleSpinBox;
            }
            layout->addWidget(m_pSpinBoxX, 1);
            layout->addWidget(m_pSpinBoxY, 1);
            layout->addWidget(m_pSpinBoxZ, 1);
            connect(m_pSpinBoxX, &QDoubleSpinBox::valueChanged, this, &Vector3PropertyEditorEntry::onEditorValueChanged);
            connect(m_pSpinBoxY, &QDoubleSpinBox::valueChanged, this, &Vector3PropertyEditorEntry::onEditorValueChanged);
            connect(m_pSpinBoxZ, &QDoubleSpinBox::valueChanged, this, &Vector3PropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    void Vector3PropertyEditorEntry::setValue(const Vector3& value) const {
        m_pSpinBoxX->setValue(value.x);
        m_pSpinBoxY->setValue(value.y);
        m_pSpinBoxZ->setValue(value.z);
    }
    Vector3 Vector3PropertyEditorEntry::getValue() const {
        return Vector3(
            m_pSpinBoxX->value(),
            m_pSpinBoxY->value(),
            m_pSpinBoxZ->value()
        );
    }

    void Vector3PropertyEditorEntry::setMinValue(const Vector3& min) const {
        m_pSpinBoxX->setMinimum(min.x);
        m_pSpinBoxY->setMinimum(min.y);
        m_pSpinBoxZ->setMinimum(min.z);
    }
    Vector3 Vector3PropertyEditorEntry::getMinValue() const {
        return Vector3(
            m_pSpinBoxX->minimum(),
            m_pSpinBoxY->minimum(),
            m_pSpinBoxZ->minimum()
        );
    }

    void Vector3PropertyEditorEntry::setMaxValue(const Vector3& max) const {
        m_pSpinBoxX->setMaximum(max.x);
        m_pSpinBoxY->setMaximum(max.y);
        m_pSpinBoxZ->setMaximum(max.z);
    }
    Vector3 Vector3PropertyEditorEntry::getMaxValue() const {
        return Vector3(
            m_pSpinBoxX->maximum(),
            m_pSpinBoxY->maximum(),
            m_pSpinBoxZ->maximum()
        );
    }

    Vector4PropertyEditorEntry::Vector4PropertyEditorEntry(const String& name, const Vector4& defaultValue, const Vector4& min, const Vector4& max, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pSpinBoxX(new QDoubleSpinBox), m_pSpinBoxY(new QDoubleSpinBox), m_pSpinBoxZ(new QDoubleSpinBox), m_pSpinBoxW(new QDoubleSpinBox) {
        m_pSpinBoxX->setValue(defaultValue.x);
        m_pSpinBoxX->setMinimum(min.x);
        m_pSpinBoxX->setMaximum(max.x);
        m_pSpinBoxY->setValue(defaultValue.y);
        m_pSpinBoxY->setMinimum(min.y);
        m_pSpinBoxY->setMaximum(max.y);
        m_pSpinBoxZ->setValue(defaultValue.z);
        m_pSpinBoxZ->setMinimum(min.z);
        m_pSpinBoxZ->setMaximum(max.z);
        m_pSpinBoxW->setValue(defaultValue.w);
        m_pSpinBoxW->setMinimum(min.w);
        m_pSpinBoxW->setMaximum(max.w);
    }

    bool Vector4PropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pSpinBoxX == nullptr) {
                m_pSpinBoxX = new QDoubleSpinBox;
            }
            if (m_pSpinBoxY == nullptr) {
                m_pSpinBoxY = new QDoubleSpinBox;
            }
            if (m_pSpinBoxZ == nullptr) {
                m_pSpinBoxZ = new QDoubleSpinBox;
            }
            if (m_pSpinBoxW == nullptr) {
                m_pSpinBoxW = new QDoubleSpinBox;
            }
            layout->addWidget(m_pSpinBoxX, 1);
            layout->addWidget(m_pSpinBoxY, 1);
            layout->addWidget(m_pSpinBoxZ, 1);
            layout->addWidget(m_pSpinBoxW, 1);
            connect(m_pSpinBoxX, &QDoubleSpinBox::valueChanged, this, &Vector4PropertyEditorEntry::onEditorValueChanged);
            connect(m_pSpinBoxY, &QDoubleSpinBox::valueChanged, this, &Vector4PropertyEditorEntry::onEditorValueChanged);
            connect(m_pSpinBoxZ, &QDoubleSpinBox::valueChanged, this, &Vector4PropertyEditorEntry::onEditorValueChanged);
            connect(m_pSpinBoxW, &QDoubleSpinBox::valueChanged, this, &Vector4PropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    void Vector4PropertyEditorEntry::setValue(const Vector4& value) const {
        m_pSpinBoxX->setValue(value.x);
        m_pSpinBoxY->setValue(value.y);
        m_pSpinBoxZ->setValue(value.z);
        m_pSpinBoxW->setValue(value.w);
    }
    Vector4 Vector4PropertyEditorEntry::getValue() const {
        return Vector4(
            m_pSpinBoxX->value(),
            m_pSpinBoxY->value(),
            m_pSpinBoxZ->value(),
            m_pSpinBoxW->value()
        );
    }

    void Vector4PropertyEditorEntry::setMinValue(const Vector4& min) const {
        m_pSpinBoxX->setMinimum(min.x);
        m_pSpinBoxY->setMinimum(min.y);
        m_pSpinBoxZ->setMinimum(min.z);
        m_pSpinBoxW->setMinimum(min.w);
    }
    Vector4 Vector4PropertyEditorEntry::getMinValue() const {
        return Vector4(
            m_pSpinBoxX->minimum(),
            m_pSpinBoxY->minimum(),
            m_pSpinBoxZ->minimum(),
            m_pSpinBoxW->minimum()
        );
    }

    void Vector4PropertyEditorEntry::setMaxValue(const Vector4& max) const {
        m_pSpinBoxX->setMaximum(max.x);
        m_pSpinBoxY->setMaximum(max.y);
        m_pSpinBoxZ->setMaximum(max.z);
        m_pSpinBoxW->setMaximum(max.w);
    }
    Vector4 Vector4PropertyEditorEntry::getMaxValue() const {
        return Vector4(
            m_pSpinBoxX->maximum(),
            m_pSpinBoxY->maximum(),
            m_pSpinBoxZ->maximum(),
            m_pSpinBoxW->maximum()
        );
    }

    ColorPropertyEditorEntry::ColorPropertyEditorEntry(const String& name, const Color& defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pColorButton(new ColorButton(Color())), m_pLabel(new QLineEdit) {
        m_pColorButton->setColor(defaultValue);
        m_pLabel->setEnabled(false);
        m_pLabel->setText(std::format("{}; {}; {}; {}", defaultValue.r, defaultValue.g, defaultValue.b, defaultValue.a).c_str());
    }

    bool ColorPropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pColorButton == nullptr) {
                m_pColorButton = new ColorButton(Color());
            }
            if (m_pLabel == nullptr) {
                m_pLabel = new QLineEdit;
            }
            layout->addWidget(m_pColorButton, 1);
            layout->addWidget(m_pLabel, 3);
            connect(m_pColorButton, &ColorButton::colorChanged, this, &ColorPropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    void ColorPropertyEditorEntry::setValue(const Color& value) const {
        m_pColorButton->setColor(value);
    }
    Color ColorPropertyEditorEntry::getValue() const {
        return m_pColorButton->color();
    }

    void ColorPropertyEditorEntry::onEditorValueChanged() {
        const auto& value = m_pColorButton->color();
        PropertyEditorEntry::onEditorValueChanged();
        m_pLabel->setText(std::format("{}; {}; {}; {}", value.r, value.g, value.b, value.a).c_str());
    }

    ArrayPropertyEditorEntry::ArrayPropertyEditorEntry(const String& name, const Vector<String>& values, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pVBoxLayout(new QVBoxLayout), m_pList(new QTableWidget), m_pAddButton(new QToolButton) {
        m_pList->setColumnCount(2);
        m_pAddButton->setText("Add item");
        m_pAddButton->setIcon(LoadEditorIcon("Editor/icons/buttons/add.png"));
        m_pAddButton->setToolTip("Add item");
        setValues(values);
    }

    bool ArrayPropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pVBoxLayout == nullptr) {
                m_pVBoxLayout = new QVBoxLayout;
            }
            if (m_pList == nullptr) {
                m_pList = new QTableWidget;
            }
            if (m_pAddButton == nullptr) {
                m_pAddButton = new QToolButton;
            }
            m_pVBoxLayout->addWidget(m_pList);
            m_pVBoxLayout->addWidget(m_pAddButton);
            m_pVBoxLayout->addStretch();
            connect(m_pAddButton, &QToolButton::triggered, this, [this] {
                addValue("");
            });
            layout->addLayout(m_pVBoxLayout);
            connect(m_pList, &QTableWidget::itemChanged, this, &ArrayPropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    void ArrayPropertyEditorEntry::setValues(const Vector<String>& values) const {
        clearValues();
        for (const auto& value : values) {
            addValue(value);
        }
    }
    Vector<String> ArrayPropertyEditorEntry::getValues() const {
        Vector<String> values;
        for (int i = 0; i < m_pList->rowCount(); i++) {
            values.emplace_back(m_pList->item(i, 0)->text().toStdString());
        }
        return values;
    }

    void ArrayPropertyEditorEntry::addValue(const String& value) const {
        const auto count = m_pList->rowCount();
        m_pList->insertRow(count);
        auto removeBtn = new QToolButton;
        removeBtn->setIcon(LoadEditorIcon("Editor/icons/buttons/delete.png"));
        removeBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        removeBtn->setToolTip("Remove item");
        connect(removeBtn, &QToolButton::clicked, this, [this, &removeBtn] {
            m_pList->removeRow(m_pList->indexAt(removeBtn->pos()).row());
        });
        m_pList->item(count, 0)->setText(value.as_cstr());
        m_pList->setCellWidget(count, 1, std::move(removeBtn));
    }
    void ArrayPropertyEditorEntry::addValues(const Vector<String>& values) const {
        for (const auto& value : values) {
            addValue(value);
        }
    }
    void ArrayPropertyEditorEntry::removeValue(const int idx) const {
        m_pList->removeRow(idx);
    }
    void ArrayPropertyEditorEntry::removeValues(const int idx, const int count) const {
        for (int i = idx; i < idx + count; ++i) {
            removeValue(i);
        }
    }
    void ArrayPropertyEditorEntry::clearValues() const {
        m_pList->clear();
    }

    MapPropertyEditorEntry::MapPropertyEditorEntry(const String& name, const HashMap<String, String>& values, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_pVBoxLayout(new QVBoxLayout), m_pList(new QTableWidget), m_pAddButton(new QToolButton) {
        m_pList->setColumnCount(3);
        m_pAddButton->setText("Add item");
        m_pAddButton->setIcon(LoadEditorIcon("Editor/icons/buttons/add.png"));
        m_pAddButton->setToolTip("Add item");
        setValues(values);
        connect(m_pList, &QTableWidget::itemChanged, this, &MapPropertyEditorEntry::validateItem);
    }

    bool MapPropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pVBoxLayout == nullptr) {
                m_pVBoxLayout = new QVBoxLayout;
            }
            if (m_pList == nullptr) {
                m_pList = new QTableWidget;
            }
            if (m_pAddButton == nullptr) {
                m_pAddButton = new QToolButton;
            }
            m_pVBoxLayout->addWidget(m_pList);
            m_pVBoxLayout->addWidget(m_pAddButton);
            m_pVBoxLayout->addStretch();
            connect(m_pAddButton, &QToolButton::triggered, this, [this] {
                addValue("", "");
            });
            layout->addLayout(m_pVBoxLayout);
        }
        return true;
    }

    void MapPropertyEditorEntry::setValues(const HashMap<String, String>& values) const {
        clearValues();
        for (const auto& [ name, value ] : values) {
            addValue(name, value);
        }
    }
    HashMap<String, String> MapPropertyEditorEntry::getValues() const {
        HashMap<String, String> values;
        for (int i = 0; i < m_pList->rowCount(); i++) {
            const auto key   = m_pList->item(i, 0)->text();
            const auto value = m_pList->item(i, 1)->text();
            values.emplace(key.toStdString(), value.toStdString());
        }
        return values;
    }

    void MapPropertyEditorEntry::addValue(const String& key, const String& value) const {
        if (findValue(key) < 0) {
            return;
        }

        const auto count = m_pList->rowCount();
        m_pList->insertRow(count);
        auto removeBtn = new QToolButton;
        removeBtn->setIcon(LoadEditorIcon("Editor/icons/buttons/delete.png"));
        removeBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        removeBtn->setToolTip("Remove item");
        connect(removeBtn, &QToolButton::clicked, this, [this, &removeBtn] {
            m_pList->removeRow(m_pList->indexAt(removeBtn->pos()).row());
        });
        m_pList->item(count, 0)->setText(key.as_cstr());
        m_pList->item(count, 1)->setText(value.as_cstr());
        m_pList->item(count, 1)->setData(Qt::UserRole, value.as_cstr());
        m_pList->setCellWidget(count, 2, std::move(removeBtn));
    }

    void MapPropertyEditorEntry::removeValue(const String& key) const {
        if (const auto idx = findValue(key); idx >= 0) {
            removeValue(idx);
        }
    }

    void MapPropertyEditorEntry::clearValues() const {
        m_pList->clear();
    }

    int MapPropertyEditorEntry::findValue(const String& key) const {
        for (int i = 0; i < m_pList->rowCount(); i++) {
            if (m_pList->item(i, 0)->text() == key.as_cstr()) {
                return i;
            }
        }
        return -1;
    }

    void MapPropertyEditorEntry::removeValue(const int idx) const {
        m_pList->removeRow(idx);
    }

    void MapPropertyEditorEntry::validateItem(QTableWidgetItem* item) {
        if (item == nullptr || item->row() == 0) return;

        const auto newValue = item->text();
        const auto oldValue = item->data(Qt::UserRole).toString();

        if (newValue.isEmpty() && oldValue.isEmpty()) return;

        for (int i = 0; i < m_pList->rowCount(); ++i) {
            if (i != item->row() && m_pList->item(i, 0)->text() == newValue.toStdString()) {
                QMessageBox::critical(this, "Failed to set value!", std::format(
                    "Cannot change keyword \"{}\" to \"{}\", keyword already exists in map!",
                    oldValue.toStdString(), newValue.toStdString()
                ).c_str());
                item->setText(oldValue);
                return;
            }
        }

        item->setData(Qt::UserRole, newValue);
        Q_EMIT editorValueChanged();
    }

    AssetPropertyEditorEntry::AssetPropertyEditorEntry(const String& name, const String& defaultValue, const Path& assetBaseDir, const DataMappingEntry& mapping_entry, QWidget* parent)
        : PropertyEditorEntry(name, mapping_entry, parent), m_sAssetBaseDir(assetBaseDir), m_pHBoxLayout(new QHBoxLayout), m_pLineEdit(new QLineEdit), m_pAddButton(new QToolButton), m_pBrowseButton(new QToolButton) {
        m_pLineEdit->setText(defaultValue.as_cstr());
        m_pAddButton->setIcon(LoadEditorIcon("Editor/icons/buttons/add.png"));
        m_pAddButton->setToolTip("Add item");
        m_pAddButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_pBrowseButton->setText("...");
        m_pBrowseButton->setToolTip("Browse item");
        connect(m_pAddButton, &QToolButton::clicked, this, &AssetPropertyEditorEntry::addAsset);
        connect(m_pBrowseButton, &QToolButton::clicked, this, &AssetPropertyEditorEntry::browseAsset);
        connect(m_pLineEdit, &QLineEdit::textChanged, this, &AssetPropertyEditorEntry::onEditorValueChanged);
    }

    bool AssetPropertyEditorEntry::createEntry(QHBoxLayout* layout) {
        if (!m_bEntryCreated) {
            if (m_pHBoxLayout == nullptr) {
                m_pHBoxLayout = new QHBoxLayout;
            }
            if (m_pLineEdit == nullptr) {
                m_pLineEdit = new QLineEdit;
            }
            if (m_pAddButton == nullptr) {
                m_pAddButton = new QToolButton;
            }
            if (m_pBrowseButton == nullptr) {
                m_pBrowseButton = new QToolButton;
            }

            m_pHBoxLayout->addWidget(m_pLineEdit,     6);
            m_pHBoxLayout->addWidget(m_pBrowseButton, 1);
            m_pHBoxLayout->addWidget(m_pAddButton,    1);
            layout->addLayout(m_pHBoxLayout, 4);
            connect(m_pLineEdit, &QLineEdit::textChanged, this, &AssetPropertyEditorEntry::onEditorValueChanged);
        }
        return true;
    }

    void AssetPropertyEditorEntry::setValue(const String& values) const {
        m_pLineEdit->setText(values.as_cstr());
    }
    String AssetPropertyEditorEntry::getValue() const {
        return m_pLineEdit->text().toStdString();
    }

    void AssetPropertyEditorEntry::onEditorValueChanged() {
        PropertyEditorEntry::onEditorValueChanged();
    }

    void AssetPropertyEditorEntry::addAsset() {

    }
    void AssetPropertyEditorEntry::browseAsset() const {
        if (auto dialog = ArchiveFileSelectDialog(std::format("Select \"{}\" asset", mapping().assetClassName), m_sAssetBaseDir, "", mapping().assetClassName); dialog.exec() == QDialog::Accepted) {
            m_pLineEdit->setText(dialog.selectedPath().as_cstr());
        }
    }

    PropertyEditor::PropertyEditor(QWidget* parent) : QWidget(parent), m_pParent(static_cast<AssetPackEditor*>(parent)), m_sAssetPath("") {
        auto layout = new QVBoxLayout;
        auto button_layout = new QHBoxLayout;
        m_pSaveButton = new QToolButton;
        m_pSaveButton->setIcon(LoadEditorIcon("Editor/icons/menu/save_small.png"));
        m_pSaveButton->setToolTip("Save text");
        m_pSaveButton->setEnabled(false);
        m_pRevertButton = new QToolButton;
        m_pRevertButton->setIcon(LoadEditorIcon("Editor/icons/menu/refresh.png"));
        m_pRevertButton->setToolTip("Revert changes");
        m_pRevertButton->setEnabled(false);
        button_layout->addWidget(m_pSaveButton);
        button_layout->addWidget(m_pRevertButton);
        button_layout->addStretch();
        layout->addLayout(std::move(button_layout), 1);

        auto preview_layout = new QHBoxLayout;
        m_pPreviewArea = new QScrollArea;
        m_pPreviewArea->setLayout(std::move(new QHBoxLayout));
        m_pPreviewArea->setVisible(false);
        m_pScrollArea = new QScrollArea;
        m_pParamsLayout = new QVBoxLayout;
        m_pScrollArea->setLayout(m_pParamsLayout);
        preview_layout->addWidget(m_pPreviewArea, 4);
        preview_layout->addWidget(m_pScrollArea, 8);
        layout->addLayout(std::move(preview_layout));
        setLayout(std::move(layout));

        connect(m_pSaveButton, &QToolButton::clicked, this, &PropertyEditor::saveAsset);
        connect(m_pRevertButton, &QToolButton::clicked, this, &PropertyEditor::revertAsset);
    }

    PropertyEditor::PropertyEditor(const Path& path, const DataMapping& mappings, QWidget* parent) : PropertyEditor(parent) {
        inspectAsset(path, mappings);
    }

    void PropertyEditor::inspectAsset(const Path& path, const String& mappings) {
        const auto result = DataMapping::Load(mappings);
        if (result.has_value()) {
            inspectAsset(path, result.value());
            return;
        }
        QMessageBox::critical(this, "Mapping error", std::format("Failed to load mapping for class \"{}\": {}", mappings, result.error().message).c_str());
    }

    void PropertyEditor::inspectAsset(const Path& path, const DataMapping& mappings) {
        pugi::xml_document doc;
        doc.load_file(path.as_cstr());
        auto root_node = doc.child(mappings.getClassName().as_cstr());

        m_sAssetPath = path;
        m_mappings = mappings;
        QtHelpers::ClearLayout(m_pParamsLayout);

        const auto project_path = m_pParent->getProjectBaseDir();

        if (mappings.getClassName() == "Texture") {
            QtHelpers::ClearLayout(m_pPreviewArea->layout());
            m_pPreviewArea->setVisible(true);
            m_pImageViewer = new ImageViewer;
            m_pPreviewArea->layout()->addWidget(m_pImageViewer);
        } else {
            m_pPreviewArea->setVisible(false);
        }

        for (const auto& name: mappings.getEntryOrder()) {
            const auto& entry = mappings.getEntries().at(name);

            String value;
            if (entry.isAttribute) {
                auto attrib = root_node.attribute(name.as_cstr());
                value = attrib ? attrib.value() : entry.defaultValue;
            } else {
                auto node = root_node.child(name.as_cstr());
                value = node ? node.child_value() : entry.defaultValue;
            }

            auto display_name = entry.displayName.is_empty() ? name : entry.displayName;

            PropertyEditorEntry* widget;
            switch (entry.type) {
                case DataMappingType::Bool:       { widget = new BooleanPropertyEditorEntry(display_name, StringToBool(value).value_or(false), entry);        } break;
                case DataMappingType::Int:        { widget = new IntPropertyEditorEntry    (display_name, StringToInt<int>(value).value_or(0), entry);        } break;
                case DataMappingType::Float:      { widget = new FloatPropertyEditorEntry  (display_name, StringToFloat<double>(value).value_or(0.0), entry); } break;
                case DataMappingType::String:     { widget = new StringPropertyEditorEntry (display_name, value, entry);               } break;
                case DataMappingType::Choices:    { widget = new ChoicesPropertyEditorEntry(display_name, value, entry.values, entry); } break;
                case DataMappingType::Vector2:    { widget = new Vector2PropertyEditorEntry(display_name, StringToVec2(value).value_or(Vector2(0.0)),entry); } break;
                case DataMappingType::Vector3:    { widget = new Vector3PropertyEditorEntry(display_name, StringToVec3(value).value_or(Vector3(0.0)),entry); } break;
                case DataMappingType::Vector4:    { widget = new Vector4PropertyEditorEntry(display_name, StringToVec4(value).value_or(Vector4(0.0)),entry); } break;
                case DataMappingType::Color:      { widget = new ColorPropertyEditorEntry  (display_name, StringToColor(value).value_or(Color()), entry);    } break;
                case DataMappingType::Array: {
                    auto values = Vector<String>();
                    if (auto node = root_node.child(name.as_cstr()); node) {
                        for (const auto& child : node.children()) {
                            values.emplace_back(child.value());
                        }
                    } else {
                        values = entry.values;
                    }
                    widget = new ArrayPropertyEditorEntry(display_name, values, entry);
                } break;
                case DataMappingType::Map: {
                    auto values = HashMap<String, String>();
                    if (auto node = root_node.child(name.as_cstr()); node) {
                        for (const auto& child : node.children()) {
                            values.emplace(child.name(), child.value());
                        }
                    } else {
                        values = VectorsToHashMap(entry.keys, entry.values);
                    }
                    widget = new MapPropertyEditorEntry(display_name, entry);
                } break;
                case DataMappingType::Asset: {
                    widget = new AssetPropertyEditorEntry(display_name, value, project_path, entry);
                    if (mappings.getClassName() == "Texture") {
                        m_pImageViewer->setImage(project_path / value);
                        connect(widget, &PropertyEditorEntry::editorValueChanged, this, [this, &widget, &project_path] {
                            m_pImageViewer->setImage(
                                project_path / static_cast<AssetPropertyEditorEntry*>(widget)->getValue()
                            );
                        });
                    }
                } break;
                case DataMappingType::ShaderName: {
                    widget = new ChoicesPropertyEditorEntry(display_name, value, ShaderLib::GetShaders(), entry);
                } break;
                default: widget = nullptr; break;
            }
            if (widget != nullptr) {
                widget->setup();
                m_pParamsLayout->addWidget(widget);
                connect(widget, &PropertyEditorEntry::editorValueChanged, this, [this] {
                    m_pSaveButton->setEnabled(true);
                    m_pRevertButton->setEnabled(true);
                });
            }
        }
        m_pParamsLayout->addStretch();
    }

    void PropertyEditor::saveAsset() const {
        m_pSaveButton->setEnabled(false);
        m_pRevertButton->setEnabled(false);
    }
    void PropertyEditor::revertAsset() {
        if (QMessageBox::question(this, "Confirm revert", "Would you like to revert changes?") == QMessageBox::Yes) {
            inspectAsset(m_sAssetPath, m_mappings);
        }
        m_pSaveButton->setEnabled(false);
        m_pRevertButton->setEnabled(false);
    }
}
