import os

if os.path.exists(".gitmodules"):
    os.system("sed -i 's/git@github.com:/https:\/\/github.com\//' .gitmodules")
