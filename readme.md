# bmig 0.3.3 by [b](https://github.com/ebrian)

bmig is a simple, clean, blazing fast mysql migration manager written in c

### install dependencies

__# ubuntu__
```
sudo apt-get install gcc libmysqlclient-dev libjson0-dev pkg-config
```

__# rhel__
```
sudo yum install gcc mysql-devel json-c-devel
```

__# macos (using [homebrew] (http://brew.sh))__
```
brew install gcc json-c mysql-connector-c pkg-config
```

---

### clone & compile (linux & macos)

```
git clone https://github.com/ebrian/bmig
cd bmig
make
sudo make install
```

### clone & run (windows)

```
git clone https://github.com/ebrian/bmig
cd bmig\dist\windows-x86
bmig.exe
```

---

### bmig commands

__general bmig guidelines__
- to use bmig, you must be in the folder with your config.json file.

---

__initialize bmig__
```
bmig init
```
this will ask you a few questions and create a `config.json` for you such that there isn't one already in the folder. it will also attempt to create the `migrations/` folder.

---

__create a new migration__
```
bmig create [name]
```
this will create a new file in the format of `[timestamp]-[name].sql` in the migrations folder or the folder you've configured in your `config.json`. it will contain an `up:` and `down:` label. there must be a newline after each label and a newline after each command you write. if you don't have an `up` or a `down`, you can leave the label out of the migration, or leave it blank; either way is fine.

---

__check the migration status__
```
bmig status
```
this will check each migration in the migrations folder or the folder you've configured in your `config.json` and compare that against what is in the `zzzzzbmigmigrations` table that `bmig` will create in the beginning. migrations that are present in the table will be marked as `up` and migrations that are not will be marked as `dn`.

---

__process all pending migrations__
```
bmig migrate
```
this will run each migration marked as `dn` from `bmig status` and execute the contents from the `up:` label located in that migration. if multiple migrations are marked as `dn`, they will be run sequentially.

---

__rollback last migration__
```
bmig rollback
```
this will rollback each migration marked as `up` from `bmig status` and execute the contents from the `down:` label located in that migration. only the most recently migration will be rolled back.

---

### changelog
__0.3.3__ -- 2016-04-17
  * fix error appending junk to the generated config.json file

__0.3.2__ -- 2015-11-19
  * added final support for the -t (transaction) flag
  * added support for delimiters to do migrations with triggers, procedures, etc.

__0.3.1__ -- 2015-11-18
  * fixed instability relating to overflown remote mig

__0.3.0__ -- 2015-11-08
  * initialization script

__0.2.0__ -- 2015-11-07
  * stability fixes
  * windows support

__0.1.0__ -- 2015-10-26
  * initial release
  * version bump
  * docs clean up

__0.0.1-rc3__ -- 2015-10-26
  * migrations commands run in their own connection

__0.0.1-rc2__ -- 2015-10-14
  * adhering to stricter compiler warnings
  * additional stability fixes
  * macos support
  * fix bug with terminal color not resetting

__0.0.1-rc1__ -- 2015-10-12
  * create new migrations
  * list migration status
  * migrate all
  * rollback one
  * config.json supported
  * some error checking

---

### special thanks
ryan ferons ([@rferons](https://github.com/rferons)) -- continued moral support

shannon warren ([@w2pc](https://github.com/w2pc)) -- for testing my products even if they suck

corey edwards ([@cedwardsmedia](https://www.twitter.com/cedwardsmedia)) -- macos build procedures & testing

---

### license
bmig is available under the MIT License
