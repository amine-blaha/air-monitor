from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from flask_cors import CORS, cross_origin
from sqlalchemy import func
from flask_marshmallow import Marshmallow
import json
import os


# Initialisation de lapplication
app = Flask(__name__)
CORS(app, support_credentials=True)


basedir = os.path.abspath(os.path.dirname(__file__))
# Definir le chemin vers la base de donnée
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///' + os.path.join(basedir, 'db.sqlite')
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False


# Initialisation de la base de donnée
db = SQLAlchemy(app)
# Init ma
ma = Marshmallow(app)

# Creation du model de la table co_data sous forme de classe
class CoData(db.Model):
  id = db.Column(db.Integer, primary_key=True)
  ppm = db.Column(db.Integer)
  lat = db.Column(db.Float)
  long = db.Column(db.Float)
  

  def __init__(self, ppm, lat, long):
    self.ppm = ppm
    self.lat = lat
    self.long = long

# Model Schema
class CoDataSchema(ma.Schema):
  class Meta:
    fields = ('id', 'ppm', 'lat', 'long')

# Initialisation du schema
co_schema = CoDataSchema(strict=True)
co_data_schema = CoDataSchema(many=True, strict=True)

# creation de la route 'get_data' pour le service REST
@cross_origin(supports_credentials=True)
@app.route('/get_data', methods=['GET'])
def get_data():

  all_data = CoData.query.all()
  result = co_data_schema.dump(all_data)
  # dans cette fonction on va suivre les étapes pour le calcul du AQI
  
  # calcul de la moyenne des prélèvements effectués
  average_ppm = db.session.query(db.func.avg(CoData.ppm)).scalar()/10

  # avoir la latitude et la longitude on prend la première valeur car le GPS est
  # beaucoup sensible et peut donner différentes valeurs pour le meme point
  coordinates = db.session.query(CoData).distinct(CoData.lat, CoData.long).first()


  # obtenir les points d'arrets pour la concentration et l'AQI selon la moyenne
  min_ppm, max_ppm, min_aqi, max_aqi = get_min_max(average_ppm)

  # calcul de l'AQI selon sa formule
  aqi = int(round(aqi_calculation(average_ppm, min_ppm, max_ppm, min_aqi, max_aqi)))

  # obtention des informations relatives à l'AQI calculé
  color, level, heath_implication = aqi_information(aqi)

  # creation du string au format de l'objet JSON qui comprends toutes les informations 
  aqi_data = {"aqi":aqi, "lat":coordinates.lat, "long":coordinates.long, "color":color, "level":level, "health_implication":heath_implication}
  
  # retourner l'objet en format json 
  return jsonify(aqi_data)

# fonction qui retourne deux points d'arrets (concentration et AQI)
# selon la moyenne obtenue lors du prélèvement
def get_min_max(average_ppm):

  if(average_ppm>=0 and average_ppm<4.5):
    min_ppm = 0
    max_ppm = 4.4
    min_aqi = 0
    max_aqi = 50
    return min_ppm, max_ppm, min_aqi, max_aqi

  else:
    if(average_ppm>=4.5 and average_ppm<9.5):
      min_ppm = 4.5
      max_ppm = 9.4
      min_aqi = 51
      max_aqi = 100
      return min_ppm, max_ppm, min_aqi, max_aqi
    
    else:
      if(average_ppm>=9.5 and average_ppm<12.5):
        min_ppm = 9.5
        max_ppm = 12.4
        min_aqi = 101
        max_aqi = 150
        return min_ppm, max_ppm, min_aqi, max_aqi
      
      else:
        if(average_ppm>=12.5 and average_ppm<15.5):
          min_ppm = 12.5
          max_ppm = 15.4
          min_aqi = 151
          max_aqi = 200
          return min_ppm, max_ppm, min_aqi, max_aqi
        
        else:
          if(average_ppm>=15.5 and average_ppm>=30.5):
            min_ppm = 15.5
            max_ppm = 30.4
            min_aqi = 201
            max_aqi = 300
            return min_ppm, max_ppm, min_aqi, max_aqi

          else:
            if(average_ppm>=30.5):
              min_ppm = 30.5
              max_ppm = average_ppm
              min_aqi = 301
              max_aqi = 500
              return min_ppm, max_ppm, min_aqi, max_aqi

# fonction qui calcule l'AQI (voir mémoire)
def aqi_calculation(average_ppm, min_ppm, max_ppm, min_aqi, max_aqi):
  print(average_ppm, min_ppm, max_ppm, min_aqi, max_aqi)
  aqi = (((average_ppm - min_ppm) * (max_aqi - min_aqi)) / (max_ppm - min_ppm)) + min_aqi

  return aqi

# fonction qui donne des informations selon l'intervalle ou l'AQI se situe
def aqi_information(aqi):

  if(aqi>=0 and aqi<=50):

    color = 'green'
    level = 'Bonne'
    heath_implication = 'La qualité de l\'air est jugée satisfaisante et la pollution de l\'air pose peu ou pas de risque'
    return color, level, heath_implication

  else:
    if(aqi>=51 and aqi<=100):
      color = 'yellow'
      level = 'Significative'
      heath_implication = 'La qualité de l\'air est acceptable. cependant, pour certains polluants, il peut y avoir un problème de santé modéré pour un très petit nombre de personnes inhabituellement sensibles à la pollution atmosphérique.'
      return color, level, heath_implication
    
    else:
      if(aqi>=101 and aqi<=150):
        color = 'orange'
        level = 'Marquée pour les groupes sensibles'
        heath_implication = 'Les membres des groupes sensibles peuvent avoir des effets secondaire sur la santé. Le grand public ne sera probablement pas affecté.'
        return color, level, heath_implication
      
      else:
        if(aqi>=151 and aqi<=200):
          color = 'red'
          level = 'Mauvaise pour la santé'
          heath_implication = 'Tout le monde peut commencer à ressentir des effets sur la santé; les membres de groupes sensibles peuvent avoir des effets plus graves.'
          return color, level, heath_implication
        
        else:
          if(aqi>=201 and aqi<=300):
            color = 'purple'
            level = 'Très malsaine'
            heath_implication = 'Avertissements sanitaires relatifs aux conditions d\'urgence. Toute la population est plus susceptible d\'être touchée'
            return color, level, heath_implication
          else:
            if(aqi>=301):
              color = 'black'
              level = 'Hasardeuse'
              heath_implication = 'Alerte santé: tout le monde peut avoir des effets plus graves sur la santé.'
              return color, level, heath_implication



if __name__ == '__main__':
    app.run(debug=True)