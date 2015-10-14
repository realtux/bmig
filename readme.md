## bmig 0.0.1 by b

bmig is a simple, clean, and blazing fast mysql migration manager written in c

__install dependencies__

__# ubuntu__
```
sudo apt-get install libmysqlclient-dev libjson0-dev
```

__# rhel__
```
sudo yum install mysql-devel json-c-devel
```

__# os x (using [homebrew] (http://brew.sh))__
```
brew install gcc json-c mysql-connector-c
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
