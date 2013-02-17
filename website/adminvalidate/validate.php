<?
  
if (!isset($_GET["md5"]))
     echo "param_error";
else
{
	try {
		$conn = new PDO('mysql:host=localhost;dbname=npppm', 'npppmvalidate', 'npppmvalidate');
		
	} catch (Exception $e){
	
		echo "connect_error";
		exit;
	}

    $md5 = $_GET["md5"];
    $sql = "select status from FileHash where md5sum=:md5";
    $query = $conn->prepare($sql);
    $query->execute(array(':md5' => $_GET['md5']));
    
    $row = $query->fetch();
	 
     
    if ($row == FALSE)
	    echo "unknown";
    else if ($row['status'] == "ok" || $row['status'] == "banned")
	    echo $row['status'];
    else
	    echo "unknown";
    
  }
