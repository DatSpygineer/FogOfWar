#include "gtest/gtest.h"
#include "fow/Shared/String.hpp"

using namespace fow;
namespace fs = std::filesystem;

#ifdef _WIN32
    #define GLOBAL_PATH_TEST "C:\\Windows"
    #define PATH_SEPARATOR "\\"
    #define ROOT_PATH "C:\\"
#else
    #define GLOBAL_PATH_TEST "/usr/local"
    #define PATH_SEPARATOR "/"
    #define ROOT_PATH "/"
#endif

TEST(Path, Constructors) {
    Path path { };                                 // Empty path
    EXPECT_TRUE(path.is_empty());
    path = Path { GLOBAL_PATH_TEST };         // From c string
    EXPECT_STREQ(path.as_cstr(), GLOBAL_PATH_TEST);
    path = Path { String(GLOBAL_PATH_TEST) }; // From String class
    EXPECT_STREQ(path.as_cstr(), GLOBAL_PATH_TEST);
    path = Path { fs::current_path() };            // From fs::path class
    EXPECT_STREQ(path.as_cstr(), fs::current_path().string().c_str());
    path = Path::Join({ "a", "b", "c" });
    EXPECT_STREQ(path.as_cstr(), (fs::path("a") / fs::path("b") / fs::path("c")).string().c_str());
    EXPECT_EQ(Path::CurrentDir().as_std_path(), fs::current_path());
}

TEST(Path, PathManipulation) {
    Path path { "test" };
    path.to_absolute();
    EXPECT_STREQ(path.as_cstr(), fs::absolute("test").string().c_str());

    path = Path { "test" } / "another_test";
    EXPECT_STREQ(path.as_cstr(), (fs::path("test") / "another_test").c_str());
    path.go_back();
    EXPECT_STREQ(path.as_cstr(), "test");
    path /= "another_test";
    EXPECT_STREQ(path.as_cstr(), (fs::path("test") / "another_test").c_str());
    path += ".txt";
    EXPECT_STREQ(path.as_cstr(), (fs::path("test") / "another_test.txt").c_str());

    path = Path { "test/test" } + ".txt";
    EXPECT_STREQ(path.as_cstr(), "test" PATH_SEPARATOR "test.txt");
    path.replace_extension(".cpp");
    EXPECT_STREQ(path.as_cstr(), "test" PATH_SEPARATOR "test.cpp");
    path.replace_filename("main");
    EXPECT_STREQ(path.as_cstr(), "test" PATH_SEPARATOR "main.cpp");
    path.replace_filename_and_extension("test.txt");
    EXPECT_STREQ(path.as_cstr(), "test" PATH_SEPARATOR "test.txt");

    path = Path { ROOT_PATH "A/B/C" };
    path.replace_root(ROOT_PATH "A", ROOT_PATH "D");
    EXPECT_STREQ(path.as_cstr(), ROOT_PATH "D" PATH_SEPARATOR "B" PATH_SEPARATOR "C");
    path.to_relative(ROOT_PATH "D");
    EXPECT_STREQ(path.as_cstr(), "B" PATH_SEPARATOR "C");
    path.to_absolute();
    EXPECT_STREQ(path.as_cstr(), fs::absolute(fs::path("B" PATH_SEPARATOR "C")).string().c_str());
    path.to_relative();
    EXPECT_STREQ(path.as_cstr(), "B" PATH_SEPARATOR "C");
    path.go_back();
    EXPECT_STREQ(path.as_cstr(), "B");
}

TEST(Path, ExtractComponents) {
    const auto path = Path { ROOT_PATH "A/B/C/test.txt" };
    EXPECT_EQ(path.parent(), path.root());
    EXPECT_EQ(path.parent(), Path(ROOT_PATH "A/B/C"));
    EXPECT_EQ(path.filename(), "test.txt");
    EXPECT_EQ(path.filename_without_extension(), "test");
    EXPECT_EQ(path.extension(), ".txt");

    auto [ root, filename ] = path.split();
    EXPECT_EQ(root, path.root());
    EXPECT_EQ(filename, path.filename());

    auto [ filename2, ext ] = path.split_ext();
    EXPECT_EQ(filename2, path.filename_without_extension());
    EXPECT_EQ(ext, path.extension());

    EXPECT_TRUE(path.is_absolute());
    EXPECT_TRUE(path.matches("*.*"));
    EXPECT_TRUE(path.matches("*.txt"));
    EXPECT_FALSE(path.matches("*.cpp"));

    const auto path2 = Path { "A/B/C/test.txt" };
    EXPECT_FALSE(path2.is_absolute());
}

TEST(Path, FileSystem) {
    const auto a = Path::CurrentDir() / "directory_that_shall_not_exists";
    const auto b = Path::CurrentDir() / "directory_that_shall_not_exists_as_well";

    fs::remove_all(a.as_std_path());
    fs::remove_all(b.as_std_path());

    EXPECT_FALSE(a.exists());
    EXPECT_FALSE(b.exists());

    Path::CreateDirectory(a);
    EXPECT_TRUE(a.exists());
    EXPECT_TRUE(a.is_directory());
    EXPECT_FALSE(a.is_file());

    Path::Move(a, b);
    EXPECT_FALSE(a.exists());
    EXPECT_FALSE(a.is_directory());
    EXPECT_TRUE(b.exists());
    EXPECT_TRUE(b.is_directory());
    EXPECT_FALSE(b.is_file());
    Path::Copy(b, a);
    EXPECT_TRUE(a.exists());
    EXPECT_TRUE(a.is_directory());

    Path::Remove(b);
    EXPECT_FALSE(b.exists());
    EXPECT_FALSE(b.is_directory());

    const auto c = a / "test.txt";
    FILE* f = fopen(c.as_cstr(), "w");
    fputs("Hello!", f);
    fflush(f);
    fclose(f);
    EXPECT_TRUE(c.exists());
    EXPECT_TRUE(c.is_file());
    EXPECT_FALSE(c.is_directory());

    EXPECT_FALSE(Path::Remove(a));
    EXPECT_TRUE(Path::Remove(a, true));
    EXPECT_FALSE(c.exists());
    EXPECT_FALSE(c.is_file());
    EXPECT_FALSE(a.exists());
    EXPECT_FALSE(a.is_file());

    const auto d = a / "nested_directory";
    EXPECT_FALSE(Path::CreateDirectory(d));
    EXPECT_TRUE(Path::CreateDirectory(d, true));
    EXPECT_TRUE(d.exists());
    EXPECT_TRUE(d.is_directory());

    Path::Remove(d, true);
}