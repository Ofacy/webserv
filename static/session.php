<?php
session_start();
?>
<!DOCTYPE html>
<html>
<body>
<?php
if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST["content"])) {
	$_SESSION["POST"] = $_POST["content"];
	echo $_SESSION["POST"];
}
else if ($_SERVER['REQUEST_METHOD'] == 'GET') {
	if (isset($_SESSION["POST"]))
		echo $_SESSION["POST"];
	else
		echo "No POST data";
}
else {
	echo "Method not allowed";
}
?>
</body>
</html>