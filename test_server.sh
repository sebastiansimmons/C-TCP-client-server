URL="http://localhost:6300"

echo "curl -i -X GET $URL/key0"
curl -i -X GET $URL/key0
echo

echo "curl -i -X PUT $URL/key1/SpiderDashMan"
curl -i -X PUT $URL/key1/Spider-Man
echo

echo "curl -X GET $URL/key1"
curl -i -X GET $URL/key1
echo