#!/bin/bash

function main() {
    # Check git status
    if ! [[ -z "$(git status --porcelain)" ]]; then
        echo "There are changes or additions that have not been commited or stashed."
        echo "Exiting..."
        exit 1
    fi

    # Check git branch
    BRANCH="$(git rev-parse --abbrev-ref HEAD)"
    if [[ "$BRANCH" != "develop" ]]; then
        echo 'Please ensure you are on the develop branch.';
        echo "Exiting..."
        exit 1;
    fi

    echo -n "Input version number (vX.X.X): "
    read versionNum

    # Check if tag already exists
    if [ $(git tag -l $versionNum) ]; then
        echo "This version already exists."
        echo "Exiting..."
        exit 1
    fi

    echo -n "Input codename (if any): "
    read codeName

    # Confirmation
    read -p "Are you sure? " -n 1 -r
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Exiting..."
        exit 1
    fi

    # Changing constants.hpp
    echo -e "\nUpdating constants.hpp..."
    sed -i 's/const std::string VERSION .*/const std::string VERSION = "'"$versionNum"'";/' src/constants.hpp
    sed -i 's/const std::string CODENAME .*/const std::string CODENAME = "'"$codeName"'";/' src/constants.hpp

    # Creating tag, commit and pushing
    git add src/constants.hpp
    git commit -m '[SIC-0] Updating version and codename. '$codeName' '$versionNum'.'
    git tag $versionNum
    git push origin $versionNum
}

main