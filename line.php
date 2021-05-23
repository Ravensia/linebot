 <?php
  

function send_LINE($msg){
 $access_token = 'tsbXaQekiNu6zAetb98px1gMdY36SI2UVzLCPrzT5tdBzlAVonEMowGXXbiAI3ipHrx6pc4xe0ar+VOCbAON9YElKKi3S/kyE5z4z8GEKYXBiozUGo85gUkjCaSh50OEfGanqrQ2vVpnXssANDH0ewdB04t89/1O/w1cDnyilFU='; 

  $messages = [
        'type' => 'text',
        'text' => $msg
        //'text' => $text
      ];

      // Make a POST Request to Messaging API to reply to sender
      $url = 'https://api.line.me/v2/bot/message/push';
      $data = [

        'to' => 'Uc927d2cf98c969337e16248f02f907a7',
        'messages' => [$messages],
      ];
      $post = json_encode($data);
      $headers = array('Content-Type: application/json', 'Authorization: Bearer ' . $access_token);

      $ch = curl_init($url);
      curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
      curl_setopt($ch, CURLOPT_POSTFIELDS, $post);
      curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
      curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
      $result = curl_exec($ch);
      curl_close($ch);

      echo $result . "\r\n"; 
}

?>
