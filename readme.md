## bmig 0.0.1 by b

bmig is a simple, clean, blazing fast mysql migration manager written in c

__install dependencies__

__# ubuntu__
```
sudo apt-get install gcc libmysqlclient-dev libjson0-dev
```

__# rhel__
```
sudo yum install gcc mysql-devel json-c-devel
```

__# os x (using [homebrew] (http://brew.sh))__
```
brew install gcc json-c mysql-connector-c pkg-config
```

__clone & compile__
```
git clone https://github.com/ebrian/bmig
cd bmig
make
sudo make install
```
### bmig commands

##### important: to use bmig, you must be in the folder with your config.json file

create a new migration
```
bmig create mynewmigration
```

check the migration status
```
bmig status
```

process all pending migrations
```
bmig migrate
```

rollback last migration
```
bmig rollback
```

### changelog
0.1.0 (2015-10-26)
  * initial release
  * version bump
  * docs clean up

0.0.1-rc3 (2015-10-26)
  * migrations commands run in their own connection

0.0.1-rc2 (2015-10-14)
  * adhering to stricter compiler warnings
  * additional stability fixes
  * os x support
  * fix bug with terminal color not resetting

0.0.1-rc1 (2015-10-12)
  * create new migrations
  * list migration status
  * migrate all
  * rollback one
  * config.json supported
  * some error checking

### special thanks
corey edwards ([@cedwardsmedia](https://www.twitter.com/cedwardsmedia)) -- os x build procedures & testing

### license
bmig is available under the MIT License
