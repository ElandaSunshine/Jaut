/**
    ===============================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
    
    Copyright (c) 2019 ElandaSunshine
    ===============================================================
    
    @author Elanda (elanda@elandasunshine.xyz)
    @file   appdata.h
    @date   04, May 2019
    
    ===============================================================
 */

#pragma once

#include <jaut/expo.h>

#include <unordered_map>
#include <unordered_set>

/**
    Wraps information about a certain directory.

    This class is part of the AppData class to create a managed app structure with which you can organise your app's
    file system usage a lot better and easier.
    That also means that you can't use this class without a valid AppData instance.
    You can only construct invalid Directory objects yourself.
    However, if you need a valid one you need an AppData object.

    This class also keeps track of its parent directories as long as they exist.

    @see AppData
 */
namespace jaut
{
class JAUT_API Directory
{
public:
    //=================================================================================================================
    using t_sub_map    = juce::HashMap<juce::String, Directory>;
    using t_iterator   = t_sub_map::Iterator;
    using t_file_vec   = std::vector<juce::File>;
    using t_search_set = std::unordered_set<juce::String>;
    using t_file_type  = juce::File::TypesOfFileToFind;

    //=================================================================================================================
    /** Constructs an invalid Directory object. */
    Directory() = default;

    /**
        Constructs a new Directory object on the given path with the given name.

        @param name    The directory name
        @param baseDir The root path where this directory sits in
     */
    explicit Directory(const String &name, const String &baseDir) noexcept;

    //=================================================================================================================
    /**
        Gets a sub Directory object by its name (case-insensitive) in this Directory object when it was indexed before.
        If not, it will add a new Directory object and return it.

        You can also fetch sub Directory objects with this parameter with the only difference that this won't add a sub
        Directory object at a level higher than 1 when it does not exist.
        Neither the first sub Directory object if it does not exist and a nested path was given.

        If the last one doesn't exist but the parent does, it will return the parent Directory object.
        For example:
            directoryInstance["subfolder/subsubfgetParentDirectorylder/anotherfolder"];
            'anotherfolder' doesn't exist, here,getParentDirectoryhave at least a bit of subsubfolder for today!

        Nesting works with either '\\' or '/' e.g. 'subdir/subsubdir' or 'subdir\\subsubdir' (won't add any Directory
        objects if any of these does not exist)

        @param dirName The relative path name of the Directory object starting at this directory
        @returns       The Directory object if it was found or an invalid if none was found
     */
    Directory operator[](const String &dirName) noexcept;

    /**
        Compares the memory address of two Directory objects.

        Since Directory objects are not copyable and only valid if they are children of an AppData object, comparing
        them by pointer is perfectly fine.

        @param dir The Directory object to compare with this one
        @returns   True if the memory address is the same
     */
    const bool operator==(const Directory &dir) const noexcept;

    /**
        Compares the memory address of two Directory objects.

        Since Directory objects are not copyable and only valid if they are children of an AppData object, comparing
        them by pointer is perfectly fine.

        @param dir The Directory object to compare with this one
        @returns   True if the memory address is not the same
     */
    const bool operator!=(const Directory &dir) const noexcept;

    /* DISABLED FOR STYLE REASONS
    const bool operator<(const Directory &dir)  const noexcept;
    const bool operator<=(const Directory &dir) const noexcept;
    // */

    /**
        Checks whether this Directory object is a parent of the given Directory object.

        This only checks whether this is a parent of the Directory object at any level of the Directory tree.
        If you want to know if this Directory object is a direct parent of the given Directory object use
        the + operator instead!

        @param dir The Directory object which is supposed to be a child directory
        @returns   True if this Directory object is a parent of the given Directory object
     */
    const bool operator>(const Directory &dir)  const noexcept;

    /**
        Checks whether this Directory object is a parent of the given Directory object or if they are the same.

        This only checks whether this is a parent of the object at any level of the Directory tree.
        If you want to know if this object is a direct parent of the given Directory object use
        the += operator instead!

        @param dir The Directory object which is supposed to be a child directory
        @returns   True if this Directory is a parent of the given Directory object or when they are the same
     */
    const bool operator>=(const Directory &dir) const noexcept;

    /**
        Checks whether this Directory object is direct parent of the given Directory object.

        This only checks whether this is a direct parent of the object.
        If you want to know if this object is a parent at any level use the > operator instead!

        @param dir The Directory object which is supposed to be a parent directory or the same
        @returns True if this Directory object is a child or the same of the given Directory object
     */
    const bool operator+(const Directory &dir) const noexcept;

    /**
        Checks whether this Directory object is direct parent of the given Directory object or if they are the same.

        This only checks whether this is a direct parent of the object.
        If you want to know if this object is a parent at any level use the >= operator instead!

        @param dir The Directory object which is supposed to be a parent directory or the same
        @returns True if this Directory object is a child or the same of the given Directory object
     */
    const bool operator+=(const Directory &dir) const noexcept;

    /**getParentDirectory
        Checks whether this Directory object isgetParentDirectory a parent of the given Directory object or vice-versa.
getParentDirectory
        @param dir The Directory object which is supposed to be either a child or  a parent directory
        @returns   True if this Directory object is either a parent or a child of the given Directory object
     */
    const bool operator*(const Directory &dir) const noexcept;

    /**
        Checks whether this Directory object is a parent of the given Directory object or vice-versa or if they
        are the same.

        @param dir The Directory object which is supposed to be either a child, a parent or the same directory
        @returns   True if this Directory object is either a parent, a child of the given Directory object or if
        they are the same
     */
    const bool operator*=(const Directory &dir) const noexcept;

    /**
        Checks whether this Directory shares the same parent Directory with the given Directory object.

        @param dir The Directory object to check
        @returns   True if both Directory objects have the same parent Directory
     */
    const bool operator/(const Directory &dir) const noexcept;

    /**
        Checks whether this Directory object shares the same parent Directory with the given Directory or if
        they are the same.

        @param dir The Directory object to check
        @returns   True if both Directory objects have the same parent Directory object or if they are the same
     */
    const bool operator/=(const Directory &dir) const noexcept;

    //=================================================================================================================
    /**
        Adds a new sub Directory object to the Directory tree.

        This method returns the instance of the current Directory object so that this method can be chained.

        @param directoryName Name of the Directory object
        @returns             A reference of this Directory object
     */
    Directory withSubFolder(const String &directoryName) noexcept;

    /**
        Adds a new sub Directory object to the Directory tree if it doesn't exist already.

        Other than withSubFolder(), this returns the instance to the newly created Directory object and
        cannot be chained.

        @param directoryName Name of the Directory object
        @returns             A reference to the newly created Directory object
     */
    Directory makeDir(const String &directoryName);

    /**
        Gets a Directory object if it exists.

        @param directoryName Name of the directory
        @returns The Directory object if it exists else an invalid Directory object will be returned
     */
    const Directory getDir(const String &directoryName) const noexcept;

    //=================================================================================================================
    /**
        Checks whether this Directory object belongs to a valid AppData object or not.

        @returns True if this is a valid Directory object
     */
    const bool isValidDirectory() const noexcept;

    /**
        Checks whether this directory is a root directory or not.

        Note: This doesn't have anything to do with your filesystem's root directory.
              It just checks whether this directory has a valid parent Directory object,
              if not, it's treated like a root directory for your directory structure!

        @return True if this directory is a root directory, false if not
     */
    const bool isRootDirectory() const noexcept;

    /**
        Checks whether this Directory already exists on the file system or not.

        @returns True if the directory exists
     */
    const bool exists() const noexcept;

    /**
        Checks whether this Directory object has sub Directory objects or not.

        @returns True if this object has sub Directory objects
     */
    const bool hasSubDirectories() const noexcept;

    /**
        Checks whether this Directory object has a sub Directory object with the given name or not.

        @param name The name of the sub Directory object
        @returns    True if this Directory object contains a sub Directory object with that name
     */
    const bool hasSubDirectory(const String &name) const noexcept;

    /**
        Checks whether this directory has a valid parent Directory object or not.

        @return True if there is a valid parent Directory object, false if not
     */
    const bool hasParentDirectory() const noexcept;

    /**
        Gets the number of sub Directory objects in this Directory object.

        @returns The total number of sub Directory objects
     */
    const size_t numSubDirectories() const noexcept;

    //=================================================================================================================
    /**
        Gets the root path of the current Directory tree.

        @returns The root path or an empty string when the Directory was invalid
     */
    const String getBaseDir() const noexcept;

    /**
        Gets the name of this Directory object.

        @returns The name of the Directory or an empty string when the Directory was invalid
     */
    const String getDirectoryName() const noexcept;

    /**
        Gets a child file by the given name.

        This doesn't check if the file/directory exists or not so make sure to check it yourself.

        @param dirName Relative path name of the child file
        @returns       The child file or an empty file if the Directory was invalid
     */
    File getFile(const String &dirName) const noexcept;

    /**
        Gets the parent Directory object of this Directory object.

        @returns The parent Directory object or an invalid directory if this Directory is invalid
     */
    Directory getParentDirectory() const noexcept;

    //=================================================================================================================
    /**
        List all files which are contained in the directory this object represents.

        When specifiyng patterns, you can make use of wildcard characters like '?' and '*'.
        e.g. "*.png" to only search for png files.

        This will only search for files which are not directories.
        If you want to search for directories use listDirectories() instead.

        Note: This won't search for hidden files. If you need to find hidden items, use the juce::File class instead.

        @param fileNames       A list of patterns for what files to look for
        @param recursiveSearch (Optional) Whether sub directories should be scanned for files too
        @returns               A vector with the found files
     */
    t_file_vec listFiles(const t_search_set &fileNames,
                            bool recursiveSearch = false) const noexcept;

    /**
        List all directories which are contained in the directory this object represents.

        When specifiyng patterns, you can make use of wildcard characters like '?' and '*'.
        e.g. ".*" to only search for directories beginning with dots.

        This will only search for directories which are not files.
        If you want to search for files use listFiles() instead.

        Note: This won't search for hidden directories. If you need to find hidden items,
        use the juce::File class instead.

        @param directoryNames  A list of patterns for what directories to look for
        @param recursiveSearch (Optional) Whether sub directories should be scanned for directories too
        @returns               A vector with the found directories
     */
    t_file_vec listDirectories(const t_search_set &directoryNames,
                                bool recursiveSearch = false) const noexcept;

    /**
        Gets this' Directory object's path as string.

        @param absolute (Optional) Whether you want to get an absolute or root-relative path
        @returns        The path of this Directory object as string
     */
    const String toString(bool absolute = true) const noexcept;

    /**
        Creates all non-existant directories this Directory object represents.

        @param recursive (Optional) Whether directories in sub directories should be created too
     */
    void createFolderHierarchy(bool recursive = true) const noexcept;
    t_iterator begin() noexcept;
    t_iterator end() noexcept;

    /**
        Gets the path of this Directory object as File.
        This will represent the full path of the directory.

        @returns This Directory as a File
     */
    File toFile() const noexcept;

private:
    friend class AppData;

    struct SharedObject;
    using p_shared_object = std::shared_ptr<SharedObject>;

    p_shared_object data;
};


/**
    The AppData object provides an easy way to organise your app's storage structure on the computer's file system.

    You can easily manage files, add or remove directories and manipulate the behaviour of files which are part of
    your applicaton.

    @see Directory
 */
class JAUT_API AppData final : public Directory
{
public:
    //=================================================================================================================
    using t_rewrite_map = std::unordered_map<juce::String, Directory&>;
    using t_sp_loc      = juce::File::SpecialLocationType;
    using t_sub_vec     = std::vector<juce::String>;

    //=================================================================================================================
    /**
        Constructs a new AppData object.

        @param parentDir      The path where the root of the AppData object should be
        @param rootName       The name of the root folder
        @param subDirectories A list of names for subfolders which should be added
     */
    AppData(const File &parentDir, const String &rootName,
            const t_sub_vec &subDirectories);
    /**
        Constructs a new AppData object.

        @param parentDir The path where the root of the AppData object should be
        @param rootName  The name of the root folder
     */
    AppData(const File &parentDir, const String &rootName);

    /**
        Constructs a new AppData object.

        The name of the folder will be the last directory mentioned in the parentDir parameter.

        @param parentDir      The path where the root of the AppData object should be
        @param subDirectories A list of names for subfolders which should be added
     */
    AppData(const File &parentDir, const t_sub_vec &subDirectories);

    /**
        Constructs a new AppData object.

        The name of the folder will be the last directory mentioned in the parentDir parameter.

        @param parentDir The path where the root of the AppData object should be
     */
    explicit AppData(const File &parentDir);

    /**
        Constructs a new AppData object.

        The root folder will be in the user's default documents directory.

        @param rootName       The name of the root folder
        @param subDirectories A list of names for subfolders which should be added
     */
    AppData(const String &rootName, const t_sub_vec &subDirectories);

    /**
        Constructs a new AppData object.

        The root folder will be in the user's default documents directory.

        @param baseFolderName The name of the root folder
     */
    explicit AppData(const String &rootName);

    //=================================================================================================================
    /**
        Adds a new sub Directory object to the Directory tree.

        This method returns the instance of the AppData object so that this method can be chained.

        @param directoryName Name of the Directory object
        @returns             A reference to this AppData object
     */
    AppData &withSubFolder(const String &directoryName) noexcept;

    //=================================================================================================================
    /**
        Looks for any rewrites in the rewrite map and if none was found will return a subdirectory if it exists.

        @param directoryName The name of the rewrite
        @returns             The rewrite or a sub directory if no rewrite was found
     */
    Directory getSubDirOrPossibleRewrite(const String &directoryName) noexcept;

    //=================================================================================================================

    /**
        Gets the AppData location as File.

        @returns The location
     */
    File toFile() const noexcept
    {
        return dataDir;
    }

    /**
        Adds a Directory rewrite to the rewrite map.
        A rewrite is an alias for a directory in the AppData directory tree.

        For example:
        {@code
            Directory subDirAtAnyLevelBelowRoot = appData["path"]["to"]["sub dir"]; // this equals "root\path\to\sub dir"
            String rewriteName                  = "example_alias";

            // add the rewrite
            addRewrite(rewriteName, subDirAtAnyLevelBelowRoot);

            //get the rewrite
            Directory theDesiredSubDir = getSubDirOrPossibleRewrite(rewriteName); // theDesiredSubDir == subDirAtAnyLevelBelowRoot
        }

        @param rewriteName The alias of the path to the Directory object
        @param dir         The directory object to be mapped to this alias
     */
    void addRewrite(const String &rewriteName, Directory &dir) noexcept;

private:
    File dataDir;
    t_rewrite_map rewrites;

    //=================================================================================================================
    AppData() noexcept
        : AppData(File::getSpecialLocation(t_sp_loc::currentExecutableFile).getParentDirectory(),
                  File::getSpecialLocation(t_sp_loc::currentExecutableFile).getFileNameWithoutExtension())
    {}

    JUCE_DECLARE_NON_COPYABLE(AppData)
};
}
