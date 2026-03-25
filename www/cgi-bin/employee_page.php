<?php
    session_start();
?>

<!DOCTYPE html>
<html>
<head>
    <title>Employee Page</title>
</head>
<body>
    <h1> Welcome, </h1>
    <?php echo $_SESSION['username'];?>

    <p> You are a </p>
    <?php echo $_SESSION['role'];?>

    <a href="request_form.py"> Make a Request </a>

</body>
</html>