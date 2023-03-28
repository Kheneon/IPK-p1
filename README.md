# IPK-p1

## About app
- `ipkcpc` application is client for communication with dedicated calculator server
- Application is meant for UNIX systems, but can be run on Windows (more in point `Windows`)

## Message length limits
- UDP
    - Send: 254 Characters
    - Recieve: 253 Characters
- TCP
    - Send: 255 Characters (Including LF == '\n')
    - Recieve: 255 Characters (Including LF == '\n')

## Translation
- in parent directory
    ```
    $ make
    $ make run
    $ make clean
    ```
- And special make for printing DEBUG informations
    ```
    $ make debug
    ```

## Execution
```bash
$ ./ipkcpc [parameters] [options] ...
```
#### parameters
- `-h` - Host name (`IPv4` address) in from `X.X.X.X`, where `X` is in range <0,256>. 
[`0.0.0.0` by default]
- `-p` - Port for connection to the server, number in range <0,65535>.
[`2023` by default]
- `-m` - Which type of connection will be used. TCP/UDP.
[`TCP` by default]
#### stdin
- After execution, messages for the server will be taken from `STDIN`, sent by pressing `ENTER`

#### expected input
- TCP
    - First message for the server has to be `hello`
    - For calculating is message `solve`
    - End communication by `bye`
- UDP
    - There is no `hello`, `solve` or `bye`
    - Calculation is provided only by typing `expr` and pressing `ENTER`
- Syntax:
    ```
    hello = "HELLO" LF
    solve = "SOLVE" SP query LF
    result = "RESULT" SP 1*DIGIT LF
    bye = "BYE" LF
    ```
    ```
    operator = "+" / "-" / "*" / "/"
    expr = "(" operator 2*(SP expr) ")" / 1*DIGIT
    query = "(" operator 2*(SP expr) ")"
    ```
   

## Exit codes
- 0 - Program run without problem
- 1 - Wrong parameters while executing program 
- 2 - Creating socket failure
- 3 - Sending packet failure
- 4 - TCP connection could not be created
- 5 - Packet recieving failure
- 6 - Recieved OPCODE from server was not in range {0,1}
- 7 - Recieved status code was not in range {0,1}
- 8 - WSA creation failure (only for `WINDOWS OS`)
- 99 - Internal program failure (malloc, calloc, etc.)

## Details of communication
#### UDP
- `REQUEST` is send as bytes
    ```
            0                   1                   2                   3
    bites:  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
            +---------------+---------------+-------------------------------+
            |     Opcode    |Payload Length |          Payload Data         |
            |      (8)      |      (8)      |                               |
            +---------------+---------------+ - - - - - - - - - - - - - - - +
            :                     Payload Data continued ...                :
            + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
            |                     Payload Data continued ...                |
            +---------------------------------------------------------------+
    ```
- `RESPONSE` is in bytes
    ```
            0                   1                   2                   3
    bites:  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
            +---------------+---------------+---------------+---------------+
            |     Opcode    |  Status Code  |Payload Length | Payload Data  |
            |      (8)      |      (8)      |      (8)      |               |
            +---------------+---------------+---------------+ - - - - - - - +
            :                     Payload Data continued ...                :
            + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
            |                     Payload Data continued ...                |
            +---------------------------------------------------------------+
    ```
- `OPCODES` (value of byte)
    ```
    OPCODE       |   VALUE
    ---------------------------
    REQUEST      |     0
    RESPONSE     |     1
    ```

- `STATUS CODES` (value of byte)
    ```
    STATUS CODE  |   VALUE
    ---------------------------
         OK      |     0
        ERROR    |     1
    ```

#### TCP
- TCP is send as text (message format via `EXECUTION`)

## Testing
- In repository in folder `../tests/` are testing scripts and files with input and expected output
- Scripts:
    - `create-test.sh` 
        - running this script will help you to create file in exact folder, but tests itself have to by written by your own hand
        - will create files
            - `"FILE_NAME".in` for your selected input
            - `"FILE_NAME".out` in this file write expected output
            - `"FILE_NAME".errcode` for expected error code
            - `"FILE_NAME".arg` for arguments for execution `ipkcpc` [DONT CHANGE]
    - `remove-test.sh`
        - remove all files (`.in`, `.out`, `.errcode`,`.arg`) with select name
    - `run-tests.sh`
        - will rune all tests from folders in folder `../tests/`
        - operators:
            - `-m` will select which tests will be tested [tcp/udp]
            [`tcp` by default]
            - `-h` IPv4 address of server on which will be tested the tests
            [`0.0.0.0` by default]
            - `-p` on which port will be communication with server
            [`2023` by default]
        - Results of tests will be print on `stdin`
        - example of exectution
            ```bash
            $ ./run-tests.sh -m tcp -h 127.0.0.1 -p 2000
            ```
        
## Windows
- Application can run on windows, too.
- Normal `make` wont work, need to be executed by:
```bash
$ make all
```
- In IDE CLion from JetBrains you are able to execute it easilly or use this to translate:
```
$ gcc -g -std=c11 -Wall -Wextra -Werror -pedantic ipkcpc.c -o ipkcpc -lwsock32 -lws2_32
```
- And support of debug output is available as
```
$ make debug-win
```
- Execution of program on Windows is
```
$ ./ipkcpc.exe [parameters] [options]
```

## Reference links
- https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Project%201

## Source
- Program was inspired from NESFIT git:
    - https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs
- And from `educative.io`
    - https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
    - https://www.educative.io/answers/how-to-implement-udp-sockets-in-c

## LICENSE
- This program is under License GPLv3 (<https://fsf.org/>)
- or in repository in file LICENSE