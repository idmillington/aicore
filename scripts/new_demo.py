"""This script creates a new demo based on an existing demo. It copies
across all the source files to a new directory, and creates a .vcproj
file in the build directory. This should be run from the scripts directory.

Usage:
    new_demo.py base_demo new_demo

    base_demo: the demo to copy, including the chapter number, for
               example c03_kinematic.

    new_demo: the new demo name, including chapter number, for example
              c03_newkinematic.
"""
import sys
import os
import os.path
import shutil

def copyDirectory(from_path, to_path, from_name, to_name):
    """Copies across the contents of a directory, changing the names
    of files within it to match the new demo."""

    # Make sure our destination exists
    if not os.path.exists(to_path): os.mkdir(to_path)

    # Get the list of entries in this directory
    entries = os.listdir(from_path)
    for entry in entries:
        # Ignore special files
        if entry[0] == '.': continue
        src_path = os.path.join(from_path, entry)

        # If we have a directory, then recurse
        if os.path.isdir(src_path):
            copyDirectory(src_path, os.path.join(to_path, entry))

        # Otherwise copy
        else:
            to_filename = entry.replace(from_name, to_name)
            dest_path = os.path.join(to_path, to_filename)
            shutil.copy(src_path, dest_path)


def main():
    # Make sure we have the correct number of arguments
    if len(sys.argv) < 3:
        print __doc__
        sys.exit(1)

    # Get the demo name
    base_demo = sys.argv[1]
    base_chapter, base_demo_name = base_demo.split("_", 2)
    new_demo = sys.argv[2]
    new_chapter, new_demo_name = new_demo.split("_", 2)

    # Check the base demo exists and the new one doesn't
    base_path = os.path.join('..', 'src', 'demos', base_demo)
    if not os.path.isdir(base_path):
        print "Your base demo doesn't exist!"
        sys.exit(2)

    new_path = os.path.join('..', 'src', 'demos', new_demo)
    if os.path.exists(new_path):
        print "Your new demo already exists!"
        sys.exit(2)

    # Create the directory and copy across the source files, changing the
    # name of any that need it.
    copyDirectory(base_path, new_path, base_demo_name, new_demo_name)

    # Copy the project files
    buildDirectory = os.path.join("..", "build")
    projectFiles = [
        entry
        for entry in os.listdir(buildDirectory)
        if entry.startswith(base_demo)
        ]
    for projectFile in projectFiles:
        data = open(os.path.join(buildDirectory, projectFile), "r").read()
        data = data.replace(base_demo_name, new_demo_name)
        data = data.replace(base_demo, new_demo)

        newProjectFile = projectFile.replace(base_demo, new_demo)
        newf = open(os.path.join(buildDirectory, newProjectFile), 'w')
        newf.write(data)
        newf.close()


if __name__ == '__main__':
    main()
