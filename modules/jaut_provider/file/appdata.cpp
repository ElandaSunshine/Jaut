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
    @file   appdata.cpp
    @date   04, May 2019
    
    ===============================================================
 */

namespace
{
using namespace jaut;

AppData::Directory invalidDirectory;

//======================================================================================================================
StringArray generateSubDirList(const String &dirName) noexcept
{
    StringArray result;
    String::CharPointerType char_ptr = dirName.getCharPointer();
    String text = "";
    
    for (;;)
    {
        const juce_wchar current_char = char_ptr.getAndAdvance();
        
        if (current_char == 0)
        {
            break;
        }
        
        if (current_char == '\\' || current_char == '/')
        {
            const juce_wchar next_char = *char_ptr;
            
            if (next_char != 0)
            {
                if (next_char != '/' && next_char != '\\' && !text.isEmpty())
                {
                    result.add(text);
                    text.clear();
                }
                
                ++char_ptr;
            }
            else
            {
                break;
            }
        }
        else
        {
            text << current_char;
        }
    }
    
    if (!text.isEmpty())
    {
        result.add(text);
    }
    
    return result;
}

AppData::Directory &recursivelySearchForDesiredDirectory(AppData::Directory &dir, const StringArray &strArray,
                                                         int level) noexcept
{
    AppData::Directory *this_dir = &dir;
    
    if (level < strArray.size())
    {
        const String name = strArray[level++];
        
        if (dir.hasSubDirectory(name))
        {
            AppData::Directory &next_dir = dir[name];
            this_dir = &recursivelySearchForDesiredDirectory(next_dir, strArray, level);
        }
    }
    
    return *this_dir;
}

void searchOnFileSystemFor(File::TypesOfFileToFind type, File &searchRoot,
                           const AppData::Directory::SearchSet &searchPatternList, bool recursive,
                           AppData::Directory::FileVec &files) noexcept
{
    File basedirfile = searchRoot;
    
    if (basedirfile.exists())
    {
        String searchpattern = "";
        bool first = true;
        
        for (auto it = searchPatternList.begin(); it != searchPatternList.end(); ++it)
        {
            searchpattern += (first ? "*." : ";*.") + it->trim();
            first = false;
        }
        
        DirectoryIterator diriterator(basedirfile, recursive, searchpattern, type);
        
        while (diriterator.next())
        {
            (void)files.push_back(diriterator.getFile());
        }
    }
}
}

namespace jaut
{
/* ==================================================================================
 * ================================== Directory =====================================
 * ================================================================================== */

AppData::Directory::Directory(const String &name, const String &baseDir) noexcept
    : file(baseDir + "/" + name)
{}

AppData::Directory::~Directory() {}

//==============================================================================
AppData::Directory AppData::Directory::operator[](const String &dirName) noexcept
{
    if (dirName.containsAnyOf("/\\"))
    {
        StringArray dir_list;
        generateSubDirList(dir_list, dirName);

        if (dir_list.size() > 0)
        {
            if (hasSubDirectory(dirName))
            {
                return recursivelySearchForDesiredDirectory(data->subDirectories.at(dirName.trim().toLowerCase()),
                                                            dir_list, 1);
            }
        }

        return Directory();
    }

    if (!hasSubDirectory(dirName))
    {
        (void) withSubFolder(dirName);
    }

    return data->subDirectories.at(dirName.trim().toLowerCase());
}

bool AppData::Directory::operator==(const Directory &dir) const noexcept
{
    return data && data.get() == dir.data.get();
}

bool AppData::Directory::operator!=(const Directory &dir) const noexcept
{
    return !data || data.get() != dir.data.get();
}

bool AppData::Directory::operator>(const Directory &dir) const noexcept
{
    if (!data || !dir.data)
    {
        return false;
    }

    p_shared_object dirparent(dir.data->parent.lock());

    if(dirparent)
    {
        do
        {
            if (dirparent.get() == data.get())
            {
                return true;
            }
        }
        while(dirparent = dirparent->parent.lock());
    }

    return false;
}

bool AppData::Directory::operator>=(const Directory &dir) const noexcept
{
    return *this > dir || *this == dir;
}

bool AppData::Directory::operator+(const Directory &dir) const noexcept
{
    if(!data || !dir.data)
    {
        return false;
    }

    p_shared_object dirparent(dir.data->parent.lock());

    return dirparent && dirparent.get() == data.get();
}

bool AppData::Directory::operator+=(const Directory &dir) const noexcept
{
    return *this + dir || &dir == this;
}

bool AppData::Directory::operator*(const Directory &dir) const noexcept
{
    return dir > *this || *this > dir;
}

bool AppData::Directory::operator*=(const Directory &dir) const noexcept
{
    return *this * dir || &dir == this;
}

bool AppData::Directory::operator/(const Directory &dir) const noexcept
{
    if(!data || !dir.data)
    {
        return false;
    }

    p_shared_object thisparent(data->parent.lock());
    p_shared_object dirparent(dir.data->parent.lock());

    return thisparent && thisparent.get() == dirparent.get();
}

bool AppData::Directory::operator/=(const Directory &dir) const noexcept
{
    return *this / dir || &dir == this;
}

//==============================================================================
Directory AppData::Directory::withSubFolder(const String &directoryName) noexcept
{
    if(data)
    {
        bool flag = directoryName.containsAnyOf("\\/");

        // '\' and '/' characters are not allowed in the path with
        // this function call.
        jassert(!flag);

        if (!flag && !hasSubDirectory(directoryName))
        {
            Directory directory(directoryName, getBaseDir());
            directory.data->parent  = data;
            directory.data->baseDir = data->baseDir;

            data->subDirectories.emplace(directoryName.trim().toLowerCase(), std::move(directory));
        }
    }

    return *this;
}

Directory AppData::Directory::makeDir(const String &directoryName)
{
    if (directoryName.containsAnyOf("\\/") || hasSubDirectory(directoryName))
    {
        throw std::invalid_argument(String("Directory '" + directoryName + "' cannot be added to '"
                                           + data->name + "").toStdString());
    }

    (void) withSubFolder(directoryName);

    return data ? data->subDirectories.at(directoryName.trim().toLowerCase()) : Directory();
}

AppData::Directory AppData::Directory::getDir(const String &directoryName) const noexcept
{
    return hasSubDirectory(directoryName) ? data->subDirectories.at(directoryName.trim().toLowerCase()) : Directory();
}

//==============================================================================
bool AppData::Directory::isValidDirectory() const noexcept
{
    return data != nullptr;
}

bool AppData::Directory::isRootDirectory() const noexcept
{
    return data && !data->parent.lock();
}

bool AppData::Directory::exists() const noexcept
{
    return toFile().exists();
}

bool AppData::Directory::hasSubDirectories() const noexcept
{
    return numSubDirectories() > 0;
}

bool AppData::Directory::hasSubDirectory(const String &name) const noexcept
{
    return data ? data->subDirectories.find(name.trim().toLowerCase()) != data->subDirectories.end() : false;
}

bool AppData::Directory::hasParentDirectory() const noexcept
{
    return data && data->parent.lock();
}

int AppData::Directory::numSubDirectories() const noexcept
{
    return data ? data->subDirectories.size() : 0;
}

String AppData::Directory::getBaseDir() const noexcept
{
    return data ? data->baseDir : "";
}

String AppData::Directory::getDirectoryName() const noexcept
{
    return data ? data->name : "";
}

File AppData::Directory::getFile(const String &fileName) const noexcept
{
    return data ? File(toString(true) + "/" + fileName) : File();
}

Directory AppData::Directory::getParentDirectory() const noexcept
{
    if(!data)
    {
        return invalidDirectory;
    }

    Directory dir;

    if(auto ptr = data->parent.lock())
    {
        dir.data.swap(ptr);
    }

    return !dir.data ? invalidDirectory : dir;
}

//==============================================================================
AppData::Directory::FileVec AppData::Directory::listFiles(const t_search_set &fileNames, bool recursiveSearch) const noexcept
{
    if (!data)
    {
        return t_file_vec();
    }

    t_file_vec files;
    File directory = toFile();
    searchOnFileSystemFor(t_file_type::findFiles, directory, fileNames, recursiveSearch, files);

    return files;
}

AppData::Directory::FileVec AppData::Directory::listDirectories(const t_search_set &fileNames, bool recursiveSearch) const noexcept
{
    if (!data)
    {
        return t_file_vec();
    }

    t_file_vec files;
    File directory = toFile();
    searchOnFileSystemFor(t_file_type::findDirectories, directory, fileNames, recursiveSearch, files);

    return files;
}

AppData::Directory::Iterator AppData::Directory::begin() noexcept
{
    return data ? data->subDirectories.begin() : Iterator();
}

AppData::Directory::Iterator AppData::Directory::end() noexcept
{
    return data ? data->subDirectories.end() : Iterator();
}

AppData::Directory::ConstIterator AppData::Directory::begin() const noexcept
{
    return data ? data->subDirectories.begin() : ConstIterator();
}

AppData::Directory::ConstIterator AppData::Directory::end() const noexcept
{
    return data ? data->subDirectories.end() : ConstIterator();
}

AppData::Directory::ConstIterator AppData::Directory::cbegin() const noexcept
{
    return data ? data->subDirectories.begin() : ConstIterator();
}

AppData::Directory::ConstIterator AppData::Directory::cend() const noexcept
{
    return data ? data->subDirectories.end() : ConstIterator();
}

File AppData::Directory::toFile() const noexcept
{
    return data ? File(toString(true)) : File();
}

const String AppData::Directory::toString(bool absolute) const noexcept
{
    if (!data)
    {
        return "";
    }

    String full_path;
    p_shared_object sharedobject(data);

    do
    {
        full_path = sharedobject->name + (full_path.isEmpty() ? "" : "/" + full_path);
    }
    while(sharedobject = sharedobject->parent.lock());

    return absolute ? (data->baseDir + full_path) : full_path;
}

void AppData::Directory::createFolderHierarchy(bool recursive) const noexcept
{
    if (!data)
    {
        return;
    }

    for (auto &[name, dir] : data->subDirectories)
    {
        File newdir = dir.toFile();

        if (!newdir.exists())
        {
            (void) newdir.createDirectory();
        }

        if (recursive && dir.hasSubDirectories())
        {
            dir.createFolderHierarchy();
        }
    }
}

/* ==================================================================================
 * =================================== AppData ======================================
 * ================================================================================== */

AppData::AppData(const File &parentDirectory, const String &rootName, const t_sub_vec &subDirectories)
    : Directory(rootName, parentDirectory.getFullPathName())
{
    data->baseDir = parentDirectory.exists() ? parentDirectory.getFullPathName()
                    : File::getSpecialLocation(t_sp_loc::userDocumentsDirectory).getFullPathName();

    if (!data->baseDir.endsWith("/") && !rootName.startsWith("/"))
    {
        data->baseDir += "/";
    }

    dataDir = File(data->baseDir + rootName);

    if (!dataDir.exists() && !dataDir.createDirectory().wasOk())
    {
        throw exception::AppDataRootCreation();
    }

    for (auto name : subDirectories)
    {
        (void) withSubFolder(name);
    }
}

AppData::AppData(const File &parentDirectory, const t_sub_vec &subDirectories)
    : AppData(parentDirectory.getParentDirectory(), parentDirectory.getFileName(), subDirectories)
{}

AppData::AppData(const String &baseDirectoryName, const t_sub_vec &subDirectories)
    : AppData(File::getSpecialLocation(t_sp_loc::userDocumentsDirectory), baseDirectoryName, subDirectories)
{}

AppData::AppData(const File &parentDirectory, const String &baseDirectoryName)
    : AppData(parentDirectory, baseDirectoryName, t_sub_vec())
{}

AppData::AppData(const File &parentDirectory)
    : AppData(parentDirectory.getParentDirectory(), parentDirectory.getFileName(), t_sub_vec())
{}

AppData::AppData(const String &baseDirectoryName)
    : AppData(File::getSpecialLocation(t_sp_loc::userDocumentsDirectory), baseDirectoryName, t_sub_vec())
{}

//==============================================================================
AppData &AppData::withSubFolder(const String &directoryName) noexcept
{
    if(data)
    {
        bool flag = directoryName.containsAnyOf("\\/");

        /**
            '\' and '/' characters are not allowed in the path with
            this function call.
            */
        jassert(!flag);

        if (!flag && !hasSubDirectory(directoryName))
        {
            Directory directory(directoryName, data->baseDir);
            directory.data->parent  = data;
            directory.data->baseDir = data->baseDir;

            (void) data->subDirectories.emplace(directoryName.trim().toLowerCase(), std::move(directory));
        }
    }

    return *this;
}

Directory AppData::getSubDirOrPossibleRewrite(const String &directoryName) noexcept
{
    String lname = directoryName.trim().toLowerCase();

    if (rewrites.find(lname) != rewrites.end())
    {
        return rewrites.at(lname);
    }

    return hasSubDirectory(directoryName) ? getDir(directoryName) : Directory();
}

void AppData::addRewrite(const String &rewriteName, Directory &dir) noexcept
{
    String lname = rewriteName.trim().toLowerCase();

    // checking if directory is a direct child of this AppData object
    if (*this + dir && rewrites.find(lname) == rewrites.end())
    {
        (void) rewrites.emplace(lname, dir);
    }
}
}
