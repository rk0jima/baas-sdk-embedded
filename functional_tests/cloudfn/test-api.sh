#!/bin/sh

baseurl=http://localhost:8080/api
tenant=58980feb2909b20f69a18463
appId=589810262909b20f69a18467
appKey=IrqJVO5bzj0Mcbx6g415MI3F5idp2CkcNqLAGOwy

curl -v -X GET \
 -H "X-Application-Id: $appId" \
 -H "X-Application-Key: $appKey" \
 -H "Content-Type: text/plain" \
 -d "abcde" \
 $baseurl/1/$tenant/api/embedded/echoBack?order=tate\&where=here\&order=yoko
