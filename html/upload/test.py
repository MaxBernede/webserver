from bs4 import BeautifulSoup
import pandas as pd

# Charger le fichier HTML
with open('Paris' + '.html', 'r', encoding='utf-8') as file:
    soup = BeautifulSoup(file, 'html.parser')

# Trouver tous les éléments de liste
list_items = soup.find_all('div', class_='list-item')

data = []

# Parcourir chaque élément de liste pour extraire les informations
for item in list_items:
    # Nom de l'entreprise
    name_tag = item.find('span', class_='title_logo')
    name = name_tag.text.strip() if name_tag else 'N/A'
    
    # Numéro de téléphone
    tel_tag = item.find('div', class_='tel')
    phone = tel_tag.find('a').text.strip() if tel_tag else 'N/A'
    
    # Adresse
    address_tag = item.find('address')
    address = address_tag.get_text(separator=" ").strip() if address_tag else 'N/A'
    
    # Lien de la carte
    carte_tag = item.find('a', class_='carte')
    carte_link = carte_tag['href'] if carte_tag else 'N/A'
    
    # Lien vers le site
    site_tag = item.find('a', class_='btn btn-default')
    site_link = site_tag['href'] if site_tag else 'N/A'
    
    # Horaires
    schedule_tag = item.find('div', class_='schedule')
    schedule = []
    if schedule_tag:
        for li in schedule_tag.find_all('li'):
            day = li.find('div', class_='day').text.strip()
            time = li.find('div', class_='time').text.strip()
            schedule.append(f"{day}: {time}")
    schedule = "\n".join(schedule)
    
    # Ajouter les données extraites à la liste
    data.append([name, phone, address, carte_link, site_link, schedule])

# Convertir les données en DataFrame
columns = ['Nom', 'Téléphone', 'Adresse', 'Lien Carte', 'Lien Site', 'Horaires']
df = pd.DataFrame(data, columns=columns)

# Enregistrer le DataFrame dans un fichier Excel
df.to_excel('output.xlsx', index=False)

print("Les données ont été extraites et enregistrées dans 'output.xlsx'.")

