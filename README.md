# webserv

## STRUCTURE
- Class:
    + 'Server' => the HTTP server (webserv: conatain all +> server blocks, clients )
    + 'Cluster' => server block in the config file
    + 'Client' => conncted client include all his imformation (requested server block, request object, response object) get dropped when finish serving ..
    + 'Request' => contain request information of a client
    + 'Response' => same as request, contain response info

## HTTP REQUEST:
- Start line: Method, URI, HTTP-version (GET / HTTP/1.1)
- Header request entity (User-agent, host, Content-length ...)
- Empty line indicate the header end ("\r\n\r\n")
- body (optional)

## TODO

- [x] Server creation
- [x] Request parsing
- [x] POST request uploading
- [x] Response

- [ ] Merge everything

- [ ] Testing & handling
