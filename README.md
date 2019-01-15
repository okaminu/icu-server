Spyware I created in 2010, which turns into malware when compromised. Application launches two instances of servers, each instance monitors if the other is launched. If at least one instance is missing, the other shuts down the computer
Each instance monitors computer activity, primarily logging launched applications header titles. Servers also remove their entry from windows task manager, so it would be impossible to shut down them from there.
If you shut down it through command line, the pc will shut down and both instances will be running again on reboot.
