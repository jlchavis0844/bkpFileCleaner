# bkpFileCleaner
Small tool that searches given folder, and deletes files that contain a given string and deletes them if they are older than 1 month (created).

run with 3 params

<b>folderPath</b> - path to the directory that will be checked. End with "\*"  
&emsp;example : "\\\\baksrv2\mySQL Backups\*"  
<b>logPath</b> - path to where the log should be stored. End with "\\\\"  
&emsp;example : "\\\\baksrv2\mySQL Backups\\"  
<b>nameArg</b> - string that files to be considered for deletion must contain in their file name (case insensitive)  
&emsp;example : backup  
    
launch example : MySQLbkpFileCleaner.exe "\\\\baksrv2\mySQL Backups\\*" "\\\\baksrv2\mySQL Backups\\\\" Backup  
