# New projects

Considerations/useful information when creating brand new projects which depend on MKELibrary

## Project templates

Everything required for a completely new project is in `template/`. When starting a new project for the first time, copy all the contents from the correct folder and put them in your new project folder. 

## Using MKELibrary as a submodule

When you are starting a new project which depends on MKELibrary, such as code for a new board, it is important to make sure that you're always compiling with the same version of MKELibrary. There is no guarantee that the behavior or syntax of libraries won't change in the future. This shows an example of how to control that version using git submodules.

For example, let's say I'm developing code for the PDC, and I want to make sure it uses the most recent version of MKELibrary; currently, v2.0. Looking on github, I can see there is a branch called v2, and in it, a commit called v2.0 with hash `5333a13`. Inside the PDC folder, I'm going to run these commands:

```
git submodule add https://github.com/CalPolyFSAE/MKELibrary
cd MKELibrary
git checkout 5333a13
```

Now, I'm going to edit `config.txt` inside this MKELibrary folder to contain the correct path to my GNU binaries. Compiling MKELibrary here is optional- there is already a `libbsp.a` file in the folder. 

Finally, I'm going to edit `SConstruct` in the PDC folder (NOT the one in the MKELibrary folder) so that the BSP_PATH line says `BSP_PATH = 'MKELibrary/'`. At this point, running `scons` should compile the project correctly. 

Note that, now that PDC has a submodule, cloning now requires the `--recursive` flag:

```
git clone --recursive https://github.com/CalPolyFSAE/PDC
```


