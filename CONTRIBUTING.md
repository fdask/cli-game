# Setup Environment
https://github.com/conventionalcommit/commitlint/releases/

commitlint init
commitlint hooks create

# Release

On the master branch, run the below, after merging in your PR.

```
php vendor/bin/conventional-changelog --commit
```

It automatically commits and tags the release.