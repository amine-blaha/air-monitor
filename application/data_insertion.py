import sqlite3 as sqlite
import pandas as pd

# creation du dataframe à partit du fichier CSV
df = pd.read_csv('data.csv')

# creation dune connexion à la base de données
con = sqlite.connect('db.sqlite')

print (con)

# creation du curseur pour executer des requete sure la 
# base de donnéess
cur = con.cursor()

# vider la table co_data afin de la mettre à jour 
# avec les nouvelles valeurs enregistrées par le prototype
cur.execute('''DELETE FROM co_data;''')

# faire une iteration sur tout le dataframe
for index, row in df.iterrows():
    
    # enregistrer chaque ligne à partir du fichier vers la base de données
    cur.execute('''INSERT INTO co_data (ppm, lat, long) VALUES (?, ?, ?)''',(row[0], row[1], row[2]))
    con.commit() 

con.close()