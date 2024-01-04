<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    
    $assemblyCode = $_POST["comment"];

    
    $tempFile = tempnam(sys_get_temp_dir(), 'assembly_code_');
    file_put_contents($tempFile, $assemblyCode);

    
    $output = shell_exec("./your_cpp_executable < $tempFile");

    
    echo $output;

    
    unlink($tempFile);
}
?>
