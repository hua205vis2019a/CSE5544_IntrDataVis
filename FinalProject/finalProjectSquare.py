import pandas as pd
import numpy as np
import folium
import branca.colormap as cm
from folium.plugins import TimestampedGeoJson, FloatImage
import branca.colormap as cm
from math import log

# final data lists
# Format of emissionFinal, populationFinal, temperatureFinal is:
# [[country1, 1960, 1961..., 2013],
#  [country2, 1960, 1961..., 2013],
#  ...
#  [country22, 1960, 1961..., 2013]]
#
# Format of latLongFinalData is:
# [[countryname, cityname, latitude, longtitude]...]
emissionFinal, populationFinal, temperatureFinal = [], [], []
latLongFinalData = [["United States", "Washington, D.C.", 37.7916, -100.03823],
                    ["Germany","Berlin", 52.520008, 13.404954],
                    ["United Kingdom", "London", 51.507351, -0.127758],
                    ["China", "Beijing", 37.579412, 97.422540],
                    ["Russia", "Moscow", 69.15005, 120.218227],
                    ["Japan", "Tokyo", 43.0646, 141.3468],
                    ["France", "Paris", 48.856613, 2.352222],
                    ["Canada", "Ottowa", 45.421532, -75.697189],
                    ["Poland", "Warsaw", 52.229675, 21.012230],
                    ["India", "New Delhi", 28.613939, 77.209023],
                    ["Italy", "Rome", 41.902782, 12.496365],
                    ["Ukraine", "Kyiv", 50.450100, 30.523399],
                    ["Czech Republic", "Prague", 50.075539, 14.437800],
                    ["Belgium", "Brussels", 50.850346, 4.351721],
                    ["Australia", "Canberra", -28.343, 134.033],
                    ["Mexico", "Mexico city", 23.60559, -102.72992],
                    ["Spain", "Madrid", 40.416775, -3.703790],
                    ["Netherlands", "Amsterdam", 52.370216, 4.895168],
                    ["Kazakhstan", "Nur Sultan", 48.551525, 66.450980],
                    ["Iran", "Tehran", 31.8365, 54.3833],
                    ["Brazil", "Brasilia", -15.826691, -47.921822],
                    ["Romania", "Bucharest", 44.426765, 26.102537]]
latLongFinalData.sort(key=lambda x: x[0])
"""
read data
"""
# emission data 1960 - 2013
emissionRaw = pd.read_csv("emission.csv", usecols=[0, *range(210, 264)])
# emission data 1960 - 2013
populationRaw = pd.read_csv("population.csv", usecols=[0, *range(4, 58)])
# temperature 1960 - 2013
temperatureRaw = pd.read_csv("temperature.csv", usecols=[0, 2])

"""
pick countries with TOP30 sum by emission data
"""

emissionData = np.array(emissionRaw).tolist()
sumData = [sum(each[1:]) for each in emissionData]
hashTable, emissionIndex, countries = {}, [], []
for i, each in enumerate(sumData):
    hashTable[each] = i
sumData.sort(reverse=True)
for i in range(30):
    index = hashTable[sumData[i]]
    emissionIndex.append(index)
    countries.append(emissionData[index][0])

# pick TOP 22 countries manually
emissionCountries = ["United States", "Germany", "United Kingdom", "China", "Russia", "Japan", "France", "Canada", "Poland", "India", "Ukraine", "Italy", "Czechoslovakia", "Belgium", "Australia", "Mexico", "Spain", "Netherlands", "Kazakhstan", "Iran", "Brazil", "Romania"]

# final emission data
for each in emissionData:
    if each[0] in emissionCountries:
        emissionFinal.append(each)

# final population data
populationCountries = ["United States", "Germany", "United Kingdom", "China", "Russian Federation", "Japan", "France", "Canada", "Poland", "India", "Ukraine", "Italy", "Czech Republic", "Belgium", "Australia", "Mexico", "Spain", "Netherlands", "Kazakhstan", "Iran, Islamic Rep.", "Brazil", "Romania"]
populationData = np.array(populationRaw).tolist()
for each in populationData:
    if each[0] in populationCountries:
        populationFinal.append(each)
populationFinal.sort(key=lambda x: x[0])

# final temperature data
temperatureData = np.array(temperatureRaw).tolist()
for i in range(22):
    each = [temperatureData[i * 54][0]]
    for j in range(54):
        each.append(temperatureData[i * 54 + j][1])
    temperatureFinal.append(each)

minEachCountry, maxEachCountry = [], []
for each in emissionFinal:
    minEachCountry.append(min(each[1965-1959:2001-1959]))
    maxEachCountry.append(max(each[1965-1959:2001-1959]))

maxEmission, minEmission = max(maxEachCountry), min(minEachCountry)
colormap = cm.LinearColormap(colors=['blue', 'green', 'yellow', 'orange'],vmin=log(minEmission),vmax=log(maxEmission))
time = []
for i in range(1960, 2014):
    time.append(str(i))


# height: population 1:15000000
# width: temperature 1:1
def create_geojson():
    features = []
    for i in range(22):
        for j in range(54):
            feature = {
                'type': 'Feature',
                'geometry': {
                    'type': 'MultiPolygon',
                    'coordinates': [(((latLongFinalData[i][3] - temperatureFinal[i][j+1]/2, latLongFinalData[i][2] + populationFinal[i][j+1]/30000000),
                                      (latLongFinalData[i][3] + temperatureFinal[i][j+1]/2, latLongFinalData[i][2] + populationFinal[i][j+1]/30000000),
                                    (latLongFinalData[i][3] + temperatureFinal[i][j+1]/2, latLongFinalData[i][2] - populationFinal[i][j+1]/30000000),
                                    (latLongFinalData[i][3] - temperatureFinal[i][j+1]/2, latLongFinalData[i][2] - populationFinal[i][j+1]/30000000)
                                      ),)]},
                'properties': {
                    'time': time[j],
                    'style': {'color': colormap(log(emissionFinal[i][j+1]))},
                    'popup': latLongFinalData[i][0],
                    'iconstyle': {'fillOpacity': 0.3}
                }

            }
            features.append(feature)
    return features
features = create_geojson()

# map drawing
m = folium.Map(location=[41.902782, 12.496365], zoom_start=2.5)
TimestampedGeoJson(
    {'type': 'FeatureCollection',
     'features': features}
    , period='P1Y'
    , add_last_point=True
    , auto_play=False
    , loop=False
    , max_speed=1
    , loop_button=True
    , date_options='YYYY'
    , time_slider_drag_update=True
    , duration='P1M'
).add_to(m)

# minimap
minimap = folium.plugins.MiniMap()
m.add_child(minimap)

# legend
image_file = "Capture1.PNG"
FloatImage(image_file, bottom=87, left=3).add_to(m)

# colormap
colormap.caption = 'GHG Emission  (log(tonne))'
m.add_child(colormap)
m.save('mapSquare.html')

