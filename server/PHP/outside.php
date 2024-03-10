    <?php
    // Database connection parameters
    $host = 'localhost';
    $user = 'WeatherDisplay';
    $password = 'EINKDISPLAY';
    $database = 'Discord';
    $port = 3306; // MariaDB default port is 3306

    // Create a connection to the database
    $conn = new mysqli($host, $user, $password, $database, $port);

    // Check the connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    // Query to retrieve data from the database
    $sql = "SELECT Temp, Humidity, Time, DAY(Time) as Day, HOUR(Time) as Hour, Minute(Time) as Minute, WEEKDAY(Time) as Date FROM OUTSIDE ORDER BY ID DESC LIMIT 1";
    $result = $conn->query($sql);

    // Prepare an array to hold the data
    $data = array();

    // Fetch the data and add it to the array
    if ($result->num_rows > 0) {
        while ($row = $result->fetch_assoc()) {
            $data[] = array(
                'Temp' => $row['Temp'],
                'Humidity' => $row['Humidity'],
                'Time' => $row['Time'],
                'Day' => $row['Day'],
                'Hour' => $row['Hour'],
                'Minute' => $row['Minute'],
                'Date' => $row['Date']
            );
        }
    }

    // Close the database connection
    $conn->close();
    // Return the data as JSON
    header('Content-Type: application/json');
    echo json_encode($data, JSON_PRETTY_PRINT);
?>
