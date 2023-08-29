<!-- <?php
    header("Status: 200 OK");
?>

<!DOCTYPE html>
<html>
<head>
    <title>Addition Form</title>
</head>
<body>
    <form action="sum.php" method="GET">
        <label for="num1">Number 1:</label>
        <input type="number" name="num1" id="num1" required><br><br>

        <label for="num2">Number 2:</label>
        <input type="number" name="num2" id="num2" required><br><br>

        <input type="submit" value="Calculate">
    </form>

    <?php
    if (isset($_GET['num1']) && isset($_GET['num2'])) {
        // Retrieve the submitted numbers
        $num1 = $_GET['num1'];
        $num2 = $_GET['num2'];

        // Calculate the sum
        $sum = $num1 + $num2;

        // Display the result
        echo '<p style="color: green;">The sum of ' . $num1 . ' and ' . $num2 . ' is: ' . $sum . '</p>';
        while (1){}
    }
    ?>
</body>
</html>
 -->
 <!DOCTYPE html>
<html>
<head>
	<title>PHP CGI Test</title>
</head>
<body>
<?php

//Call getenv() function without argument

$env_array =getenv();

echo "<h3>The list of environment variables with values are :</h3>";

//Print all environment variable names with values

foreach ($env_array as $key=>$value)

{

    echo "$key => $value <br />";

}

?>
</body>
</html>