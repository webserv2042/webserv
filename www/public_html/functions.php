<?php

//add a new user with its hashed password and role
function add_user($username, $password, $role, $name)
{
    $file_path = '../database/users.csv';

    //hash the pw
    $hashed_password = password_hash($password, PASSWORD_DEFAULT);

    //user data row
    $user_data = [$username, $hashed_password, $role, $name];

    //add to csv file in append mode (doesnt delete previous stuff)
    if (($handle = fopen($file_path, "a")) !== FALSE)
    {
        fputcsv($handle, $user_data);
        fclose($handle);
    }
    else 
    {
        echo "Error: Could not open the file. Check folder permissions.\n";
    }
}

//empty a file
function clear_file_content($filename) {
    // 'w' opens the file for writing only and clears the content
    $file = fopen($filename, 'w');
    
    if ($file !== false) {
        fclose($file);
        return true;
    }
    return false;
}

//check if login matches
function verify_login($submitted_username, $submitted_password)
{
    $file_path = "../database/users.csv";

    // 1. Open the file for reading ('r')
    if (($file_fd = fopen($file_path, "r")) !== FALSE)
    {
        // 2. Loop through each row of the CSV
        while (($data = fgetcsv($file_fd, 1000, ",")) !== FALSE)
        {
            // Check if file has at least 3 columns
            if (count($data) < 3) continue;

            $stored_username = $data[0];
            $stored_hash = $data[1];
            $stored_role = $data[2];

            // 3. Compare the username
            if ($submitted_username === $stored_username) 
            {
                // 4. Verify the password against the hash
                if (password_verify($submitted_password, $stored_hash)) 
                {
                    fclose($file_fd);
                    return $stored_role; // Success!
                }
            }
        }
        fclose($file_fd);
    }
    return false; // User not found or password incorrect
}

?>

