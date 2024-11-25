<?php
// Récupère le contenu brut du corps de la requête
$bodyContent = file_get_contents('php://input');

// Affiche le contenu du corps de la requête
echo $bodyContent;
echo "Hello, World!";
?>