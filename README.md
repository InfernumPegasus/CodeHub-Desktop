# CodeHub
**CodeHub** is a git-like version control system.

## Features
Using CodeHub you can use key features of `git` such as:
* Create repositories
* See all created repositories
* Create and easily edit `ignore`-file
* Commit files
* Restore files
* Check condition of repository
* See commits log
* Push changes on server (server required)

## Requirements
To build this code you need to have installed:
* [nlohmann::json library](https://github.com/nlohmann/json)
* [cpr library](https://github.com/libcpr/cpr) (included in CMake as submodule)
* [boost](https://www.boost.org/) (for program_options)

## Building
This project in running by `cmake`.
You can build project by source code using this commands:
```
~/../CodeHub> mkdir release && cd release
~/../CodeHub/release> cmake -DCMAKE_BUILD_TYPE=Release ..
```

## Commands
* `--help`
```
Allowed options:
  --help                produce help message
  --init arg            init new repository
  --status              get repository status
  --repositories        show all repositories
  --log                 show commits history
  --commit arg          store changes
  --restore arg         remove file from index or get it back to committed 
                        condition
  --push                update external links and objects
```

* `--init arg`

Here, `arg` is repository name. 
Inits repository in current folder 
(if there is no another repo in this folder).

Created:
```
~/../CodeHub>Codehub --init "My best repo"
Repository 'My_best_repo' created in folder '"/home/vladimir/GitHub/CodeHub"'.
```
Not created:
```
Cannot create repository.
Use --repositories command to see all repositories.
```

* `--status`

Provides info about tracked files
```
Repository 'My_best_repo' status: 
        Changed:        "CMakeCache.txt"
        Changed:        "CMakeFiles/rules.ninja"
        Changed:        "Testing/Temporary/LastTest.log"
        Changed:        "TESTFILE"
        Changed:        "CodeHub"
        Changed:        "build.ninja"
        Created:        "IMANEWFILE"
        Deleted:        "IAMDELETED"
```

If repository is up-to-date:
```
Repository 'My_best_repo' status: Up-to-date.
```

* `--repositories`

Prints all initialised repositories in
'name' : 'folder' format:
```
'My_best_repo' : '/home/vladimir/GitHub/CodeHub/cmake-build-release'
'AMOGUS230' : '/home/vladimir/GitHub/CodeHub/cmake-build-debug'
```

* `--log`

Prints all commits in repo in current folder:
```
commit 8702022520826489699
        initial

commit 5471899063856167928
        AMOGUS

commit 5958624672764303929
        change

commit 3891149043130770149
        change 2

commit 13900782043146834854
        change
```

* `--commit ard`

Here, `arg` is commit message.
Creates in hidden config folder
another folder which is named by
commit hash and store current
files condition and structure.

```
Commit 'change'
15 creations, 7 modifications, 0 deletions
```

* `--restore arg`

Here, `arg` is commit hash.
Copies all files from commit folder and
overwrites existing.

* `--push`

Does `POST` or `PATCH` http-request
and stores repository on server.

*You cannot use this command without
special web-server!*
