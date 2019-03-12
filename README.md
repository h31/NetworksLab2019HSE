# DNS server and client

## Installation

To run build created from sources run
```
$ ./gradlew client:run
$ ./gradlew server:run
``` 

To create executables run
```
$ ./gradlew client:startShadowScripts
$ ./gradlew server:startShadowScripts
```

Path to binaries: `$PROJECT_NAME$/build/lib`, path to scripts: `$PROJECT_NAME$/build/scriptsShadow`.

To run bundled binaries run `$ ./client` or `$ ./server` from directory `dist/scripts`

## Configuration

Those domain name server implements part of DNS specification. Server is configured by config file, that supports five different records:
1. `domain IN SOA ns e-mail Serial Refresh Retry Expire DefaultTTL` main record, required
2. `domain IN NS address` domain server record
3. `name IN A address` address record
4. `сname IN CNAME name` address alias record
5. `domain IN MX address` email server record

Name servers can use different ports, so server address must be in format `ip:port`

Each server can be top-level (or not) and recursive or not-recursive. That parameters configures when servers starts.

There is an option to run multiple servers on same computer, see `server/src/main/resources/dns.preset` and `server` program for usage.
