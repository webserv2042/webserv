<?php
	include 'functions.php';

	session_start();
	
	// create users
	clear_file_content("../database/users.csv");
	add_user('manager@gmail.com', 'mpw', 'manager', 'tung tung tung tung sahur');
	add_user('employee@gmail.com', 'upw', 'employee', 'brr brrr patabim');

	// DEBUG
	$debug = file_get_contents("../database/users.csv");
	error_log("CSV CONTENT: " . $debug);
	error_log("POST DATA: " . print_r($_POST, true));

	// set variables to empty
	$email = $password = "";
	$emailErr = "";

	// fill variables
	if ($_SERVER["REQUEST_METHOD"] == "POST")
	{
		if (empty($_POST["email"]))
		{
			$_SESSION["login_error"] = "Email is required";
			header("Location: login.php");
			exit();
		}
		else 
		{
			$email = htmlspecialchars($_POST["email"]);

			if (empty($_POST["password"]))
			{
				$_SESSION["login_error"] = "Password is required";
				header("Location: login.php");
				exit();
			}
			else
			{
				$password = htmlspecialchars($_POST["password"]);

				//process login input
				$role = verify_login($email, $password);

				if ($role === 'manager')
				{
					$_SESSION['username'] = $email;
					$_SESSION['role'] = 'manager';
					// create cookies for python
					// $cookie_username = find_username($email);
					// setcookie("cookie_username", $cookie_username, time() + 3600, "/cgi-bin/manager_page.py");
					// 
					header('Location: ../cgi-bin/manager_page.py');
					exit();
				}
				else if ($role === 'employee')
				{
					$_SESSION['username'] = $email;
					$_SESSION['role'] = 'employee';
					// create cookies for python
					$cookie_username = find_username($email);
					setcookie("cookie_username", $cookie_username, time() + 3600, "/");
					// 
					header('Location: ../cgi-bin/employee_page.py');
					exit();
				}
				else
				{
					$_SESSION["login_error"] = "User not found or password incorrect";
					header("Location: login.php");
					exit();
				}
			}
				
		}
	}

	//retrieve errors
	$display_error = $_SESSION["login_error"] ?? "";
	unset($_SESSION["login_error"]);
?>

<!DOCTYPE html>
<html>
<head>
	<link rel="stylesheet" href="style.css">
	<title>Login Page</title>
</head>
<body>

	<div class="login-content">
		<div class="login-text">
			<p>Nice to see you again</p>
			<h1>Welcome Back</h1>
			<div class="divider"></div>
		</div>
		<div class="login-form">
			<h2>Login Account</h2>

			<?php if ($display_error): ?>
				<p class="error-msg"><?php echo $display_error; ?></p>
			<?php endif; ?>

			<form action="<?php echo htmlspecialchars($_SERVER['PHP_SELF']); ?>" method="post" autocomplete="off">
				<div class="input-group">
					<input type="email" name="email" placeholder="Username" maxlength="30">
				</div>
				<div class="input-group">
					<input type="password" name="password" placeholder="Password" maxlength="30">
				</div>
				<button type="submit">Sign In</button>
			</form>
		</div>
	</div>

</body>
</html>