### bmig

bmig is a simple, clean, easy to use mysql migration manager.

bmig uses the following directory structure:
```
migrator/ <- binary is in this directory
    src/
    makefile
```

make bmig
```
cd migrator
make
```

create a new migration
```
cd migrator
./migrator create mynewmigration
```

check the migration status
```
cd migrator
./migrator status
```

process all pending migrations
```
cd migrator
./migrator migrate
```

rollback last migration
```
cd migrator
./migrator rollback
```
