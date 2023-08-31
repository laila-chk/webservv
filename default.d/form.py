#!/usr/bin/python

# Import modules for CGI handling 
import cgi, cgitb 
# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
n = form.getvalue('n')
l = form.getvalue('l')
print ("<html>")
print ("<head>")
print ("<title>Hello & CGI Program</title>")
print ("</head>")
print ("<body>")
print ("<h2>Hello %s %s</h2><br>" % (n, l))
print ("</body>")
print ("</html>")
