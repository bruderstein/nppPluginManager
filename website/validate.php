<?
  define('IN_PLUGIN_MANAGER', 'true');
  
  require_once('settings.php');
  
  if (!isset($_GET["md5"]))
     echo "param_error";
  else
  {
     $conn = new PDO('sqlite:hidden/files.db3');
     
     $md5 = str_replace("'", "\\'", $_GET["md5"]);
     $sql = "select status from FileHash where md5sum='" . $md5 . "'";
     $conn->query($sql);
	 $row = $query->fetch();
	 
     
	if ($row == FALSE)
	    echo "unknown";
    else if ($row['status'] == "ok" || $row['status'] == "banned")
	    echo $row['status'];
	else
	    echo "unknown";
    
  }
?>
