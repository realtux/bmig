## bmig 0.0.1 by b

bmig is a simple, clean, easy to use mysql migration manager.

make bmig
```
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
