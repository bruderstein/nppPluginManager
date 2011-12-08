<?
  
if (!isset($_GET["md5"]))
     echo "param_error";
else
{
	try {
		$db = substr(__FILE__, 0, strlen(__FILE__) - strlen(strrchr(__FILE__, '/')))
			. '/hidden/files.db3';
		$conn = new PDO("sqlite:$db");
	} catch (Exception $e){
	
		echo "connect_error";
		exit;
	}

    $md5 = str_replace("'", "\\'", $_GET["md5"]);
    $sql = "select status from FileHash where md5sum='" . $md5 . "'";
    $query = $conn->query($sql);
    $row = $query->fetch();
	 
     
    if ($row == FALSE)
	    echo "unknown";
    else if ($row['status'] == "ok" || $row['status'] == "banned")
	    echo $row['status'];
    else
	    echo "unknown";
    
  }
?>
