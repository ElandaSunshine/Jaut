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

namespace jaut
{
/**
    The AppData object provides an easy way to organise your app's storage structure on the computer's file system.

    You can easily manage files, add or remove directories and manipulate the behaviour of files which are part of
    your applicaton.

    @see Directory
 */
class JAUT_API AppData final
{
public:
    /**
     *  Wraps information about a certain directory.
     *
     *  This class is part of the AppData class to create a managed app structure with which you can organise your app's
     *  file system usage a lot better and easier.
     *  That also means that you can't use this class without a valid AppData instance.
     *  You can only construct invalid Directory objects yourself.
     *  However, if you need a valid one you need an AppData object.
     *
     *  This class also keeps track of its parent directories as long as they exist.
     *
     *  @see AppData
     */
    class JAUT_API Directory final
    {
    public:
        //==============================================================================================================
        /** The type of vector containing files. */
        using FileVec = std::vector<juce::File>;
        
        /** The type of set containing a list of valid file extensions. */
        using SearchSet = std::unordered_set<juce::String>;
        
        //==============================================================================================================
        /** Constructs an invalid Directory object. */
        Directory() = default;
        
        /**
         *  Constructs a new Directory object on the given path with the given name.
         *
         *  @param name    The directory name
         *  @param baseDir The root path where this directory sits in
         */
        Directory(const String &name, const String &baseDir);
        
        //==============================================================================================================
        /**
         *  Compares the memory address of two Directory objects.
         *
         *  Since Directory objects are not copyable and only valid if they are children of an AppData object, comparing
         *  them by pointer is perfectly fine.
         *
         *  @param dir The Directory object to compare with this one
         *  @return True if the memory address is the same
         */
        bool operator==(const Directory &dir) const noexcept;
        
        /**
         *  Compares the memory address of two Directory objects.
         *
         *  Since Directory objects are not copyable and only valid if they are children of an AppData object, comparing
         *  them by pointer is perfectly fine.
         *
         *  @param dir The Directory object to compare with this one
         *  @return True if the memory address is not the same
         */
        bool operator!=(const Directory &dir) const noexcept;
        
        /**
         *  Checks whether this Directory object is a parent of the given Directory object.
         *
         *  This only checks whether this is a parent of the Directory object at any level of the Directory tree.
         *  If you want to know if this Directory object is a direct parent of the given Directory object use
         *  the + operator instead!
         *
         *  @param dir The Directory object which is supposed to be a child directory
         *  @return True if this Directory object is a parent of the given Directory object
         */
        bool operator>(const Directory &dir)  const noexcept;
        
        /**
         *  Checks whether this Directory object is a parent of the given Directory object or if they are the same.
         *
         *  This only checks whether this is a parent of the object at any level of the Directory tree.
         *  If you want to know if this object is a direct parent of the given Directory object use
         *  the += operator instead!
         *
         *  @param dir The Directory object which is supposed to be a child directory
         *  @return True if this Directory is a parent of the given Directory object or when they are the same
         */
        bool operator>=(const Directory &dir) const noexcept;
        
        /**
         *  Checks whether this Directory object is direct parent of the given Directory object.
         *
         *  This only checks whether this is a direct parent of the object.
         *  If you want to know if this object is a parent at any level use the > operator instead!
         *
         *  @param dir The Directory object which is supposed to be a parent directory or the same
         *  @return True if this Directory object is a child or the same of the given Directory object
         */
        bool operator+(const Directory &dir) const noexcept;
        
        /**
         *  Checks whether this Directory object is direct parent of the given Directory object or if they are the same.
         *
         *  This only checks whether this is a direct parent of the object.
         *  If you want to know if this object is a parent at any level use the >= operator instead!
         *
         *  @param dir The Directory object which is supposed to be a parent directory or the same
         *  @return True if this Directory object is a child or the same of the given Directory object
         */
        bool operator+=(const Directory &dir) const noexcept;
        
        /**
         *  Checks whether this Directory object isgetParentDirectory a parent of the given Directory object
         *  or vice-versa.
         *
         *  @param dir The Directory object which is supposed to be either a child or  a parent directory
         *  @return True if this Directory object is either a parent or a child of the given Directory object
         */
        bool operator*(const Directory &dir) const noexcept;
        
        /**
         *  Checks whether this Directory object is a parent of the given Directory object or vice-versa or if they
         *  are the same.
         *
         *  @param dir The Directory object which is supposed to be either a child, a parent or the same directory
         *  @return True if this Directory object is either a parent, a child of the given Directory object or if
         *           they are the same
         */
        bool operator*=(const Directory &dir) const noexcept;
        
        /**
         *  Checks whether this Directory shares the same parent Directory with the given Directory object.
         *
         *  @param dir The Directory object to check
         *  @return True if both Directory objects have the same parent Directory
         */
        bool operator/(const Directory &dir) const noexcept;
        
        /**
         *  Checks whether this Directory object shares the same parent Directory with the given Directory or if
         *  they are the same.
         *
         *  @param dir The Directory object to check
         *  @return True if both Directory objects have the same parent Directory object or if they are the same
         */
        bool operator/=(const Directory &dir) const noexcept;
        
        //==============================================================================================================
        /**
         *  Adds a new sub Directory object to the Directory tree.
         *
         *  This method return the instance of the current Directory object so that this method can be chained.
         *
         *  @param directoryName Name of the Directory object
         *  @return A reference of this Directory object
         */
        Directory withSubFolder(const String &directoryName) noexcept;
        
        /**
         *  Adds a new sub Directory object to the Directory tree if it doesn't exist already.
         *
         *  Other than withSubFolder(), this return the instance to the newly created Directory object and
         *  cannot be chained.
         *
         *  @param directoryName Name of the Directory object
         *  @return A reference to the newly created Directory object
         */
        Directory makeDir(const String &directoryName);
        
        /**
         *  Gets a Directory object if it exists.
         *
         *  @param directoryName Name of the directory
         *  @return The Directory object if it exists else an invalid Directory object will be returned
         */
        Directory getDir(const String &directoryName) const noexcept;
        
        //==============================================================================================================
        /**
         *  Checks whether this Directory object exists.
         *  @return True if this is a valid directory
         */
        bool isValid() const noexcept;
        
        /**
         *  Checks whether this Directory already exists on the file system or not.
         *  @return True if the directory exists
         */
        bool exists() const noexcept;
        
        /**
         *  Checks whether this Directory object has sub Directory objects or not.
         *  @return True if this object has sub Directory objects
         */
        bool hasSubDirectories() const noexcept;
        
        //==============================================================================================================
        /**
         *  Gets the name of this Directory object.
         *  @return The name of the Directory or an empty string when the Directory was invalid
         */
        String getDirectoryName() const noexcept;
        
        /**
         *  Gets a child file by the given name.
         *  This doesn't check if the file/directory exists or not so make sure to check it yourself.
         *
         *  @param dirName Relative path name of the child file
         *  @return The child file or an empty file if the Directory was invalid
         */
        File getFile(const String &dirName) const noexcept;
        
        /**
         *  Gets the parent Directory object of this Directory object.
         *  @return The parent Directory object or an invalid directory if this Directory is invalid
         */
        Directory getParentDirectory() const noexcept;
        
        //==============================================================================================================
        /**
         *  List all files which are contained in the directory this object represents.
         *
         *  When specifiyng patterns, you can make use of wildcard characters like '?' and '*'.
         *  e.g. "*.png" to only search for png files.
         *
         *  This will only search for files which are not directories.
         *  If you want to search for directories use listDirectories() instead.
         *
         *  Note: This won't search for hidden files. If you need to find hidden items, use the juce::File
         *  class instead.
         *
         *  @param fileNames       A list of patterns for what files to look for
         *  @param recursiveSearch (Optional) Whether sub directories should be scanned for files too
         *  @return A vector with the found files
         */
        FileVec listFiles(const SearchSet &fileNames, bool recursiveSearch = false) const noexcept;
        
        /**
         *  List all directories which are contained in the directory this object represents.
         *
         *  When specifiyng patterns, you can make use of wildcard characters like '?' and '*'.
         *  e.g. ".*" to only search for directories beginning with dots.
         *
         *  This will only search for directories which are not files.
         *  If you want to search for files use listFiles() instead.
         *
         *  Note: This won't search for hidden directories. If you need to find hidden items,
         *  use the juce::File class instead.
         *
         *  @param directoryNames  A list of patterns for what directories to look for
         *  @param recursiveSearch (Optional) Whether sub directories should be scanned for directories too
         *  @return A vector with the found directories
         */
        FileVec listDirectories(const SearchSet &directoryNames, bool recursiveSearch = false) const noexcept;
        
        /**
         *  Creates all non-existent directories this Directory object represents.
         *  @param recursive (Optional) Whether directories in sub directories should be created too
         */
        void createFolderHierarchy(bool recursive = true) const noexcept;
        
        //==============================================================================================================
        /**
         *  Gets this' Directory object's path as string.
         *
         *  @param absolute (Optional) Whether you want to get an absolute or root-relative path
         *  @return The path of this Directory object as string
         */
        String toString(bool absolute = true) const noexcept;
        
        /**
         *  Gets the path of this Directory object as File.
         *  This will represent the full path of the directory.
         *
         *  @return This Directory as a File
         */
        File toFile() const noexcept;
    
    private:
        File dirPath{};
    };
    
    //==================================================================================================================
    /** The type of vector containing all the sub-directories. */
    using SubDirVec = std::vector<juce::String>;
    
    //==================================================================================================================
    /**
     *  Constructs a new AppData object.
     *
     *  @param parentDir      The path where the root of the AppData object should be
     *  @param rootName       The name of the root folder
     *  @param subDirectories A list of names for subfolders which should be added
     */
    AppData(const File &parentDir, const String &rootName,
            const SubDirVec &subDirectories);
    /**
     *  Constructs a new AppData object.
     *
     *  @param parentDir The path where the root of the AppData object should be
     *  @param rootName  The name of the root folder
     */
    AppData(const File &parentDir, const String &rootName);

    /**
     *  Constructs a new AppData object.
     *  The name of the folder will be the last directory mentioned in the parentDir parameter.
     *
     *  @param parentDir      The path where the root of the AppData object should be
     *  @param subDirectories A list of names for subfolders which should be added
     */
    AppData(const File &parentDir, const SubDirVec &subDirectories);

    /**
     *  Constructs a new AppData object.
     *  The name of the folder will be the last directory mentioned in the parentDir parameter.
     *
     *  @param parentDir The path where the root of the AppData object should be
     */
    explicit AppData(const File &parentDir);

    /**
     *  Constructs a new AppData object.
     *  The root folder will be in the user's default documents directory.
     *
     *  @param rootName       The name of the root folder
     *  @param subDirectories A list of names for subfolders which should be added
     */
    AppData(const String &rootName, const SubDirVec &subDirectories);

    /**
     *  Constructs a new AppData object.
     *  The root folder will be in the user's default documents directory.
     *
     *  @param baseFolderName The name of the root folder
     */
    explicit AppData(const String &rootName);
    
    //==================================================================================================================
    /**
     *  Adds a new sub Directory object to the Directory tree.
     *  This method return the instance of the AppData object so that this method can be chained.
     *
     *  @param directoryName Name of the Directory object
     *  @return A reference to this AppData object
     */
    AppData &withSubFolder(const String &directoryName) noexcept;
    
    //==================================================================================================================
    /**
     *  Looks for any rewrites in the rewrite map and if none was found will return a subdirectory if it exists.
     *
     *  @param directoryName The name of the rewrite
     *  @return The rewrite or a sub directory if no rewrite was found
     */
    Directory getSubDirOrPossibleRewrite(const String &directoryName) noexcept;
    
    //==================================================================================================================

    /**
     *  Gets the AppData location as File.
     *  @return The location
     */
    File toFile() const noexcept;

    /**
     *  Adds a Directory rewrite to the rewrite map.
     *  A rewrite is an alias for a directory in the AppData directory tree.
     *
     *  For example:
     *  @code
     *      // this equals "root\path\to\sub dir"
     *      Directory subDirAtAnyLevelBelowRoot = appData["path"]["to"]["sub dir"];
     *      String rewriteName                  = "example_alias";
     *
     *      // add the rewrite
     *      addRewrite(rewriteName, subDirAtAnyLevelBelowRoot);
     *
     *      //get the rewrite: theDesiredSubDir == subDirAtAnyLevelBelowRoot
     *      Directory theDesiredSubDir = getSubDirOrPossibleRewrite(rewriteName);
     *  @endcode
     *
     *  @param rewriteName The alias of the path to the Directory object
     *  @param dir         The directory object to be mapped to this alias
     */
    void addRewrite(const String &rewriteName, Directory &dir) noexcept;

private:
    using RewriteMap = std::unordered_map<juce::String, File&>;
    using FileVec    = std::vector<juce::File>;
    using SpecialLoc = File::SpecialLocationType;
    
    //==================================================================================================================
    File directory;
    RewriteMap rewrites;

    //==================================================================================================================
    AppData()
        : AppData(File::getSpecialLocation(SpecialLoc::currentExecutableFile).getParentDirectory(),
                  File::getSpecialLocation(SpecialLoc::currentExecutableFile).getFileNameWithoutExtension())
    {}

    JUCE_DECLARE_NON_COPYABLE(AppData)
};
}
