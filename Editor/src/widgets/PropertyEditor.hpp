#ifndef FOW_EDITOR_PROPERTYEDITOR_HPP
#define FOW_EDITOR_PROPERTYEDITOR_HPP

#include <variant>
#include <climits>
#include <cfloat>

#include <QCheckBox>
#include <QTreeView>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QGridLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QMessageBox>

#include "pugixml.hpp"

#include <fow/Shared.hpp>

#include "GLView.hpp"
#include "widgets/ColorButton.hpp"
#include "dialogs/AssetPackEditor.hpp"
#include "util/ShaderParamInfo.hpp"

namespace fow {
    class PropertyEditor;

    inline HashMap<String, String> VectorsToHashMap(const Vector<String>& keys, const Vector<String>& values) {
        HashMap<String, String> result;
        const size_t count = keys.size() < values.size() ? keys.size() : values.size();
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.emplace(keys[i], values[i]);
        }
        return result;
    }

    enum class DataMappingType {
        Bool,        // bool
        Int,         // int
        Float,       // float
        String,      // string
        Choices,     // choices (enum)
        Vector2,     // Vector2
        Vector3,     // Vector3
        Vector4,     // Vector4
        Color,       // Color
        Array,       // Array:ItemType
        Map,         // Map:ValueType (keys are always strings)
// Special game engine specific types
        Asset,       // Asset:AssetType => Path to a game asset
        ShaderName   // ShaderName => Auto generated list of shaders
    };

    struct DataMappingEntry {
        String name;
        String displayName;
        String category;
        String assetClassName;
        String arrayItemName;
        DataMappingType type;
        DataMappingType itemType;  // Used by array and map
        String defaultValue;
        Vector<String> keys;
        Vector<String> values;
        String minValue, maxValue; // Used by numeric values
        bool optional, isAttribute;
    };

    class DataMapping {
        String m_sClassName;
        Vector<String> m_entryOrder;
        HashMap<String, DataMappingEntry> m_entries;
    public:
        DataMapping() = default;
        explicit DataMapping(const String& className) : m_sClassName(className) { }
        DataMapping(const DataMapping&) = default;
        DataMapping(DataMapping&&) noexcept = default;

        DataMapping& operator=(const DataMapping&) = default;
        DataMapping& operator=(DataMapping&&) noexcept = default;

        [[nodiscard]] FOW_CONSTEXPR bool isNull() const { return m_entries.empty(); }

        [[nodiscard]] FOW_CONSTEXPR const HashMap<String, DataMappingEntry>& getEntries() const { return m_entries; }
        [[nodiscard]] FOW_CONSTEXPR const Vector<String>& getEntryOrder() const { return m_entryOrder; }
        [[nodiscard]] FOW_CONSTEXPR const String& getClassName() const { return m_sClassName; }

        static Result<DataMapping> Load(const String& name);
        static Result<DataMapping> FromXml(const String& name, const pugi::xml_node& node);
    };

    class PropertyEditorEntry : public QWidget {
        Q_OBJECT
    protected:
        QHBoxLayout* m_pBaseLayout;
        String m_sName;
        DataMappingEntry m_mapping;
        bool m_bEntryCreated = false;
    public:
        PropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        [[nodiscard]] FOW_CONSTEXPR const String& name() const { return m_sName; }
        [[nodiscard]] FOW_CONSTEXPR const DataMappingEntry& mapping() const { return m_mapping; }
        [[nodiscard]] FOW_CONSTEXPR QHBoxLayout* getBaseLayout() const { return m_pBaseLayout; }
        [[nodiscard]] FOW_CONSTEXPR bool isEntryCreated() const { return m_bEntryCreated; }

        virtual void createLabel();
        virtual bool createEntry(QHBoxLayout* layout) = 0;
        virtual void setup();
    Q_SIGNALS:
        void editorValueChanged();
    protected:
        virtual void onEditorValueChanged() { Q_EMIT editorValueChanged(); }
    };

    class BooleanPropertyEditorEntry : public PropertyEditorEntry {
        QCheckBox* m_pCheckBox;
    public:
        inline BooleanPropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : BooleanPropertyEditorEntry(name, StringToBool(mapping_entry.defaultValue).value_or(false), mapping_entry, parent) { }
        BooleanPropertyEditorEntry(const String& name, bool defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        bool getValue() const;
        void setValue(bool value) const;
    };
    class IntPropertyEditorEntry : public PropertyEditorEntry {
        QSpinBox* m_pSpinBox;
    public:
        inline IntPropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : IntPropertyEditorEntry(name,
                StringToInt<int>(mapping_entry.defaultValue).value_or(0),
                StringToInt<int>(mapping_entry.minValue).value_or(INT_MIN), StringToInt<int>(mapping_entry.maxValue).value_or(INT_MAX),
                mapping_entry, parent
            ) { }
        inline IntPropertyEditorEntry(const String& name, const int defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : IntPropertyEditorEntry(name, defaultValue, INT_MIN, INT_MAX, mapping_entry, parent) { }
        IntPropertyEditorEntry(const String& name, int defaultValue, int min, int max, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        int getValue() const;
        void setValue(int value) const;

        void setMinValue(int min) const;
        int getMinValue() const;
        void setMaxValue(int max) const;
        int getMaxValue() const;
    };
    class FloatPropertyEditorEntry : public PropertyEditorEntry {
        QDoubleSpinBox* m_pSpinBox;
    public:
        inline FloatPropertyEditorEntry(const String& name,const DataMappingEntry& mapping_entry, QWidget* parent = nullptr) :
            FloatPropertyEditorEntry(name,
                StringToFloat<double>(mapping_entry.defaultValue).value_or(0.0),
                StringToFloat<double>(mapping_entry.minValue).value_or(DBL_MIN),
                StringToFloat<double>(mapping_entry.maxValue).value_or(DBL_MAX),
                0.001,
                mapping_entry, parent
            ) { }
        inline FloatPropertyEditorEntry(const String& name, const double defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : FloatPropertyEditorEntry(name, defaultValue, DBL_MIN, DBL_MAX, 0.001, mapping_entry, parent) { }
        inline FloatPropertyEditorEntry(const String& name, const double defaultValue, const double step, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : FloatPropertyEditorEntry(name, defaultValue, DBL_MIN, DBL_MAX, step, mapping_entry, parent) { }
        FloatPropertyEditorEntry(const String& name, double defaultValue, double min, double max, double step, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        double getValue() const;
        void setValue(double value) const;

        void setMinValue(double min) const;
        double getMinValue() const;
        void setMaxValue(double max) const;
        double getMaxValue() const;
    };
    class StringPropertyEditorEntry : public PropertyEditorEntry {
        QLineEdit* m_pLineEdit;
    public:
        StringPropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : StringPropertyEditorEntry(name, mapping_entry.defaultValue, mapping_entry, parent) { }
        StringPropertyEditorEntry(const String& name, const String& defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        String getValue() const;
        void setValue(const String& value) const;
    };
    class ChoicesPropertyEditorEntry : public PropertyEditorEntry {
        QComboBox* m_pComboBox;
    public:
        ChoicesPropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : ChoicesPropertyEditorEntry(name, mapping_entry.defaultValue, mapping_entry.values, mapping_entry, parent) { }
        ChoicesPropertyEditorEntry(const String& name, const String& defaultValue, const Vector<String>& choices, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        void setChoices(const Vector<String>& choices) const;
        void setValue(const String& value) const;
        String getValue() const;
    };
    class Vector2PropertyEditorEntry : public PropertyEditorEntry {
        QDoubleSpinBox* m_pSpinBoxX;
        QDoubleSpinBox* m_pSpinBoxY;
    public:
        Vector2PropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : Vector2PropertyEditorEntry(name,
                StringToVec2(mapping_entry.defaultValue).value_or(Vector2(0.0)),
                StringToVec2(mapping_entry.minValue).value_or(Vector2(DBL_MIN)),
                StringToVec2(mapping_entry.maxValue).value_or(Vector2(DBL_MAX)),
                mapping_entry, parent
            ) { }
        Vector2PropertyEditorEntry(const String& name, const Vector2& defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : Vector2PropertyEditorEntry(name, defaultValue, Vector2(DBL_MIN), Vector2(DBL_MAX), mapping_entry, parent) { }
        Vector2PropertyEditorEntry(const String& name, const Vector2& defaultValue, const Vector2& min, const Vector2& max, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        void setValue(const Vector2& value) const;
        Vector2 getValue() const;

        void setMinValue(const Vector2& min) const;
        Vector2 getMinValue() const;
        void setMaxValue(const Vector2& max) const;
        Vector2 getMaxValue() const;
    };
    class Vector3PropertyEditorEntry : public PropertyEditorEntry {
        QDoubleSpinBox* m_pSpinBoxX;
        QDoubleSpinBox* m_pSpinBoxY;
        QDoubleSpinBox* m_pSpinBoxZ;
    public:
        Vector3PropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : Vector3PropertyEditorEntry(name,
                StringToVec3(mapping_entry.defaultValue).value_or(Vector3(0.0)),
                StringToVec3(mapping_entry.minValue).value_or(Vector3(DBL_MIN)),
                StringToVec3(mapping_entry.maxValue).value_or(Vector3(DBL_MAX)),
                mapping_entry, parent
            ) { }
        Vector3PropertyEditorEntry(const String& name, const Vector3& defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : Vector3PropertyEditorEntry(name, defaultValue, Vector3(DBL_MIN), Vector3(DBL_MAX), mapping_entry, parent) { }
        Vector3PropertyEditorEntry(const String& name, const Vector3& defaultValue, const Vector3& min, const Vector3& max, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        void setValue(const Vector3& value) const;
        Vector3 getValue() const;

        void setMinValue(const Vector3& min) const;
        Vector3 getMinValue() const;
        void setMaxValue(const Vector3& max) const;
        Vector3 getMaxValue() const;
    };
    class Vector4PropertyEditorEntry : public PropertyEditorEntry {
        QDoubleSpinBox* m_pSpinBoxX;
        QDoubleSpinBox* m_pSpinBoxY;
        QDoubleSpinBox* m_pSpinBoxZ;
        QDoubleSpinBox* m_pSpinBoxW;
    public:
        Vector4PropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : Vector4PropertyEditorEntry(name,
                StringToVec4(mapping_entry.defaultValue).value_or(Vector4(0.0)),
                StringToVec4(mapping_entry.minValue).value_or(Vector4(DBL_MIN)),
                StringToVec4(mapping_entry.maxValue).value_or(Vector4(DBL_MAX)),
                mapping_entry, parent
            ) { }
        Vector4PropertyEditorEntry(const String& name, const Vector4& defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : Vector4PropertyEditorEntry(name, defaultValue, Vector4(DBL_MIN), Vector4(DBL_MAX), mapping_entry, parent) { }
        Vector4PropertyEditorEntry(const String& name, const Vector4& defaultValue, const Vector4& min, const Vector4& max, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        void setValue(const Vector4& value) const;
        Vector4 getValue() const;

        void setMinValue(const Vector4& min) const;
        Vector4 getMinValue() const;
        void setMaxValue(const Vector4& max) const;
        Vector4 getMaxValue() const;
    };
    class ColorPropertyEditorEntry : public PropertyEditorEntry {
        ColorButton* m_pColorButton;
        QLineEdit* m_pLabel;
    public:
        ColorPropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : ColorPropertyEditorEntry(name, StringToColor(mapping_entry.defaultValue).value_or(Color()), mapping_entry, parent) { }
        ColorPropertyEditorEntry(const String& name, const Color& defaultValue, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        void setValue(const Color& value) const;
        Color getValue() const;
    protected:
        void onEditorValueChanged() override;
    };
    class ArrayPropertyEditorEntry : public PropertyEditorEntry {
        QVBoxLayout* m_pVBoxLayout;
        QTableWidget* m_pList;
        QToolButton* m_pAddButton;
    public:
        ArrayPropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : ArrayPropertyEditorEntry(name, mapping_entry.values, mapping_entry, parent) { }
        ArrayPropertyEditorEntry(const String& name, const Vector<String>& values, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        void setValues(const Vector<String>& values) const;
        Vector<String> getValues() const;

        void addValue(const String& value) const;
        void addValues(const Vector<String>& values) const;
        void removeValue(int idx) const;
        void removeValues(int idx, int count) const;
        void clearValues() const;
    };
    class MapPropertyEditorEntry : public PropertyEditorEntry {
        QVBoxLayout* m_pVBoxLayout;
        QTableWidget* m_pList;
        QToolButton* m_pAddButton;
    public:
        MapPropertyEditorEntry(const String& name, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : MapPropertyEditorEntry(name, VectorsToHashMap(mapping_entry.keys, mapping_entry.values), mapping_entry, parent) { }
        MapPropertyEditorEntry(const String& name, const HashMap<String, String>& values, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        void setValues(const HashMap<String, String>& values) const;
        HashMap<String, String> getValues() const;

        void addValue(const String& key, const String& value) const;
        void removeValue(const String& key) const;
        void clearValues() const;

    private:
        int findValue(const String& key) const;
        void removeValue(int idx) const;
        void validateItem(QTableWidgetItem* item);
    };
    class AssetPropertyEditorEntry : public PropertyEditorEntry {
        QHBoxLayout* m_pHBoxLayout;
        QLineEdit* m_pLineEdit;
        QToolButton* m_pAddButton;
        QToolButton* m_pBrowseButton;
        Path m_sAssetBaseDir;
    public:
        AssetPropertyEditorEntry(const String& name, const Path& assetBaseDir, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr)
            : AssetPropertyEditorEntry(name, mapping_entry.defaultValue, assetBaseDir, mapping_entry, parent) { }
        AssetPropertyEditorEntry(const String& name, const String& defaultValue, const Path& assetBaseDir, const DataMappingEntry& mapping_entry, QWidget* parent = nullptr);

        bool createEntry(QHBoxLayout* layout) override;

        void setValue(const String& values) const;
        String getValue() const;
    protected:
        void onEditorValueChanged() override;
    private:
        void addAsset();
        void browseAsset() const;
    };

    class PropertyEditor : public QWidget {
        DataMapping m_mappings;
        QToolButton* m_pSaveButton;
        QToolButton* m_pRevertButton;
        Path m_sAssetPath;
        QVBoxLayout* m_pParamsLayout;
        QScrollArea* m_pScrollArea;
        QScrollArea* m_pPreviewArea;
        AssetPackEditor* m_pParent;
        union {
            ImageViewer* m_pImageViewer;
        };
    public:
        explicit PropertyEditor(QWidget* parent = nullptr);
        explicit PropertyEditor(const Path& path, const DataMapping& mappings, QWidget* parent = nullptr);

        void inspectAsset(const Path& path, const String& mappings);
        void inspectAsset(const Path& path, const DataMapping& mappings);
    private:
        void saveAsset() const;
        void revertAsset();
    };
}

#endif