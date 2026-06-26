BEGIN {
colonna = 0

#print colonna 
contatore = 0.0
medio = 0.0
somma = 0.0
}
{

evento = $colonna
contatore = contatore + 1
somma = somma + evento

print contatore, somma, somma/contatore
}
END {}
