<?php
    session_start();
?>

<!DOCTYPE html>
<html>
<head>
    <title>Manager Page</title>
</head>
<body>
    <h1> Welcome, </h1>
    <?php echo $_SESSION['username'];?>

    <p> You are a </p>
    <?php echo $_SESSION['role'];?>
</body>
</html>