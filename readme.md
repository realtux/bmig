### bmig

bmig is a simple, clean, easy to use mysql migration manager.

bmig uses the following directory structure:
```
migrations/ <- by default where new migrations are created
config.json <- your config for bmig to access the right db
```

make bmig
```
make
sudo make install
```

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
