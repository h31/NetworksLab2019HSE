# Torrent

### Description

This repository contains simple torrent with tracker and client applications.

### How to run

In order to run this app, follow these steps:

* clone this repository
* checkout on branch *torrent*
* run `gradle trackerJar` and `gradle clientJar` to build jar for TrackerApp and ClientApp respectively
* go to the `build/libs` directory
* run ```java -jar trackerApp-jar-1.0-SNAPSHOT.jar``` and ```java -jar clientApp-jar-1.0-SNAPSHOT.jar``` to run Tracker or Client respectively 

### Usage

Tracker:</br>
```
exit - shutdown the tracker app
```

Client:</br>
```
help - print usage
list - list available files on the tracker
sources <id> - list sources for file with the specified id
upload <path> - add file to the tracker
download <id> - download file with the specified id
exit - shutdown the client app
```

### File hierarchy
```
torrent/ - directory for downloads
    .metainfo/ - directory for torrent metafiles
        client/
            parts/ 
                [id]/[part] - directories named as id for parts of file
            local_files_manager_file - stored client state with info about local files
        tracker/
            tracker_state_file - stored tracker state with info about available files

``` 
