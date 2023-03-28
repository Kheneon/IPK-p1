# CHANGELOG

## Initial commit
- Initial commit was created after few commits on my personal github, so inital commit is a bit bigger
- `ADD:` Testing right shapes of switches and arguments

## First iteration
- `ADD:` TCP connection creating and sending messages
- `ADD:` UDP connection creating and sending messages
- `FIX:` lot of commits were used to repair bugs from TCP and UDP connection

## Second iteration
- `ADD:` Basic testing of source codes, testing scripts and files for tests
- `FIX:` UDP and TCP message sending repairing bugs

## Third iteration
- `ADD:` Windows support

## Fourth iteration
- `ADD:` Links for source codes that this projects was inspired from
- `ADD:` Licensing

## Fifth iteration
- `ADD:` CHANGELOG.md
- `ADD:` timeout on UDP communication

## Limitations of program
- As it is described in documenatation (`README.md`), there is a limitation for message input and output in length
- UDP
    - Send: 254 Characters
    - Recieve: 253 Characters
- TCP
    - Send: 255 Characters (Including LF == '\n')
    - Recieve: 255 Characters (Including LF == '\n')