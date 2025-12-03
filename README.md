
## This for a school project
A simple app on the command-line
that manages stocks in a way that
is kind of efficient.

# How to use??
## Registering
*You need to register* before you can use 
the feature of this app. Use this argument
to register:
### Powershell
```
.\main -register
```
### CMD
```
main -register
```
Then a menu will pop up then you can fill that up 
with your desired information.This will create a
".stock" file and you can use that to login. *Warning:* clicking 
the Esc/Enter key will finish the input process. 

## Loging in
*After registering*, You can use this command to 
login and open the menu. Make sure you have
the password and username remembered.
Then enter the file name of the *.stock* account on the arguments
of -register. No need to include the file extension.  
Example of logging in with "test.stock" file:
### Powershell
```
.\main -login test
```
### CMD
```
main -login test
```

### All the features for managing items and sales is listed.

