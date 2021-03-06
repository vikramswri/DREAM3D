# Downloading the Source Code for DREAM.3D #

**Note**: If you used the DREAM.3D Superbuild to build your SDK, you can skip this page.  The DREAM.3D Superbuild automatically downloads the source code for you.

<a name="prerequisites">
## Prerequisites ##
</a>

These prerequisites need to be completed before the source code for DREAM.3D is downloaded.

If you have already fulfilled all of these prerequisites, skip to the [Downloading the Source Code](#downloading_src_code) section.

### Install Git ###

Git needs to be installed on your system to be able to clone repositories from Github.

To install Git, please visit the [Git website](https://git-scm.com/downloads).

**Note**: If using Windows, during the installation ensure that the "Windows command prompt" can use Git.

<a name="downloading_src_code">
## Downloading the Source Code ##
</a>

DREAM.3D is publicly available on Github, and can be downloaded using [Git](http://www.git-scm.org). The user is strongly encouraged to use a GUI based Git tool such as [SourceTree](http://www.sourcetreeapp.com) in order to manage cloning all four (4) source archives.

#### Mac OS X ####
1. Make a new directory called **Workspace** in your home directory.  The **Workspace** directory's path should be **/Users/[*YOUR-HOME-DIRECTORY*]/Workspace**.

2. Open Terminal at /Applications/Utilities/Terminal.app.

3. Navigate to your Workspace directory at **/Users/[*YOUR-HOME-DIRECTORY*]/Workspace**.

4. Use git to clone the DREAM.3D repository to your **Workspace** directory:

    `git clone -b develop https://www.github.com/bluequartzsoftware/DREAM3D`

    Create and navigate to ExternalProjects directory inside DREAM3D directory

	Clone 3 other repositories to your **ExternalProjects** directory.  These repositories are required to build DREAM.3D:

    `git clone -b develop https://www.github.com/bluequartzsoftware/CMP`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPL`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPLView`

	All 4 of these repositories should be cloned/downloaded into the **Workspace** folder so that the final paths to all 4 repositories look like the following:

	/Users/[*YOUR-HOME-DIRECTORY*]/Workspace/DREAM3D
    
    /Users/[*YOUR-HOME-DIRECTORY*]/Workspace/DREAM3D/ExternalProjects/CMP
    
    /Users/[*YOUR-HOME-DIRECTORY*]/Workspace/DREAM3D/ExternalProjects/SIMPL
    
    /Users/[*YOUR-HOME-DIRECTORY*]/Workspace/DREAM3D/ExternalProjects/SIMPLView

#### Windows 10 ####

1. Make a new directory called **Workspace** in your user directory.  The **Workspace** directory's path should be **C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace**.

2. Open the git-bash application that was installed when you downloaded and installed Git.

3. Navigate to your Workspace directory at **C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace**.

4. Use git to clone the DREAM.3D repository to your **Workspace** directory:

    `git clone -b develop https://www.github.com/bluequartzsoftware/DREAM3D`

    Create and navigate to ExternalProjects directory inside DREAM3D directory

	Clone 3 other repositories to your **ExternalProjects** directory that are required to build DREAM.3D:

    `git clone -b develop https://www.github.com/bluequartzsoftware/CMP`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPL`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPLView`

	All 4 of these repositories should be cloned/downloaded into the **Workspace** folder so that the final paths to all 4 repositories look like the following:

    C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace\\DREAM3D
    
    C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace\\DREAM3D\\ExternalProjects\\CMP
    
    C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace\\DREAM3D\\ExternalProjects\\SIMPL
    
    C:\\Users\\[*YOUR-USER-DIRECTORY*]\\Workspace\\DREAM3D\\ExternalProjects\\SIMPLView

#### Linux ####
1. Make a new directory called **Workspace** in your home directory.  The **Workspace** directory's path should be **/home/[*YOUR-HOME-DIRECTORY*]/Workspace**.

2. Open the Terminal program.

3. Navigate to your Workspace directory at **/home/[*YOUR-HOME-DIRECTORY*]/Workspace**.

4. Use git to clone the DREAM.3D repository to your **Workspace** directory:

    `git clone -b develop https://www.github.com/bluequartzsoftware/DREAM3D`

	Create and navigate to ExternalProjects directory inside DREAM3D directory

    Clone 3 other repositories to your **ExternalProjects** directory.  These repositories are required to build DREAM.3D:

    `git clone -b develop https://www.github.com/bluequartzsoftware/CMP`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPL`
    `git clone -b develop https://www.github.com/bluequartzsoftware/SIMPLView`

	All 4 of these repositories should be cloned/downloaded into the **Workspace** folder so that the final paths to all 4 repositories look like the following:

	/home/[*YOUR-HOME-DIRECTORY*]/Workspace/DREAM3D
    
    /home/[*YOUR-HOME-DIRECTORY*]/Workspace/DREAM3D/ExternalProjects/CMP
    
    /home/[*YOUR-HOME-DIRECTORY*]/Workspace/DREAM3D/ExternalProjects/SIMPL
    
    /home/[*YOUR-HOME-DIRECTORY*]/Workspace/DREAM3D/ExternalProjects/SIMPLView

---
**Next Page (Windows)**: [Configuring and Building DREAM.3D on Windows](http://dream3d.bluequartz.net/binaries/Help/DREAM3D/windows_configure_and_build_dream3d.html).

**Next Page (OS X)**: [Configuring and Building DREAM.3D on Mac OS X](http://dream3d.bluequartz.net/binaries/Help/DREAM3D/osx_configure_and_build_dream3d.html).

**Next Page (Linux)**: [Configuring and Building DREAM.3D on Linux](http://dream3d.bluequartz.net/binaries/Help/DREAM3D/linux_configure_and_build_dream3d.html).

**Previous Page (Windows)**: <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_Windows.md">Making an SDK</a>

**Previous Page (OS X)**: <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_OSX.md">Making an SDK</a>

**Previous Page (Linux)**: <a href="https://github.com/bluequartzsoftware/DREAM3DSuperbuild/blob/develop/docs/Making_an_SDK_Linux.md">Making an SDK</a>