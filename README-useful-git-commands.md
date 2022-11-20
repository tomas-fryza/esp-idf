Some useful commands for git that are good to remember.

| **Command** | **Example** | **Description** |
| :-: | :-- | :-- |
| `clone`  | `git clone https://github.com/tomas-fryza/esp-idf` | Create a local copy of a remote repository |
| `pull`   | `git pull` | Update local repository and working folder to the newest commit |
| `status` | `git status` | Display the state of the working directory and the staging area |
|          | `git status -s` | Give the output in the short-format |
| `add`    | `git add main.c` | Add `main.c` file to the staging area |
|          | `git add -A` | Add all new and changed files to the staging area |
| `commit` | `git commit -m "[LAB] Changed parameter F_CPU to 1MHz"` | Commit changes, i.e. record changes to the local repository
| `push`   | `git push` | Push changes to remote repository |
| `fetch` | `git fetch` | Downloads new data from a remote repository to local repository but doesn't integrate into working files |
| `merge` | `git merge` | Approves merge changes with local repository |
| `log`    | `git log --summary` | View detailed changes |
|          | `git log --oneline` | View changes (briefly) |
| `branch` | `git branch -a` | List all local and remote branches (asterisk denotes the current branch) |
|          | `git branch -d <existing_branch>` | Delete branch |
| `checkout` | `git checkout <existing_branch>` | Switch branch |
|            | `git checkout -b <new_branch>` | Switch to new branch |
|            | `git checkout master` <br /> `git merge <existing_branch>` | Update main branch with branch's changes |

A detailed description of all git commands can be found [here](https://github.com/joshnh/Git-Commands).

Setting username and email for a single repository:
```shell
$ git config user.name "your-git-user-name"
$ git config user.email "your-email@address.com"
```

List global git parameters:
```shell
$ git config --global --list
```

Clear global username/email:
```shell
$ git config --global --unset-all user.name
$ git config --global --unset-all user.email
$ git config --global --unset-all gui.recentrepo
```
