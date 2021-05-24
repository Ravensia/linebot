<?php
$access_token = 'tsbXaQekiNu6zAetb98px1gMdY36SI2UVzLCPrzT5tdBzlAVonEMowGXXbiAI3ipHrx6pc4xe0ar+VOCbAON9YElKKi3S/kyE5z4z8GEKYXBiozUGo85gUkjCaSh50OEfGanqrQ2vVpnXssANDH0ewdB04t89/1O/w1cDnyilFU=';

$url = 'https://api.line.me/v1/oauth/verify';

$headers = array('Authorization: Bearer ' . $access_token);

$ch = curl_init($url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
$result = curl_exec($ch);
curl_close($ch);

echo $result;
