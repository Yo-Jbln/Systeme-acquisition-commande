# Systeme-acquisition-commande
Répertoire du TP de Système d'acquisition et de commande.

La documentation Doxygen se trouve dans l'archive ``Doxygen.zip``, il suffit d'extraire le dossier et ouvrir le fichier ``index.html`` pour visualiser la documentation. 

* Dans un premier temps, il est question de la création d'une console dans laquelle on rentrera des commandes pour diriger le moteur.
* Commandes disponibles :

```
help : affiche toutes les commandes disponibles
pinout : affiche toutes les broches connectées et leur fonction
start : allume l'étage de puissance du moteur
stop : éteint l'étage de puissance du moteur
speed = <alpha> : fait tourner le moteur à la vitesse voulue
```


Lors de la commande ``start`` la carte envoie un implusion au hacheur pour l'allumer et le réinitialiser.

* Nous avons en suite créé deux PWM, avec un rapport cyclique réglable, qui sont envoyées au hacheur pour que celui-ci commande notre moteur à courant continue.
Vous pouvez retrouver ci-dessous la forme des PWM utilisées, suivant le cahier des charges imposant une fréquence de 16 kHz et un temps mort de 2 µs minimum.

![image](https://user-images.githubusercontent.com/86347317/141968295-56410056-098b-42b9-88f9-33dc8b2ca38a.png)

* Puis nous avons fait la liste des Pins à utiliser à l'aide de la documentation du hacheur.
* Liste des Pins :
```
Pin 9 = 5V
Pin 12 = TIM1_CH2N (phase jaune)
Pin 13 = TIM1_CH1 (phase rouge)
Pin 16 = Capteur de courant (phase jaune)
Pin 18 = GND
Pin 19 = 5V
Pin 27 = GND
Pin 30 = TIM1_CH2 (phase jaune)
Pin 31 = TIM1_CH1N (phase rouge)
Pin 33 = Reset (impulsion) PA0
Pin 35 = Capteur de courant (phase rouge)
Pin 36 = GND
Pin 37 = 5V
```

* Une fois le tout branché, nous avons essayé d'envoyé nos PWM avec les rapports cycliques de 50%, 70%, 100% et 0%. Le problème que nous avons rencontré est que le changement brutal du rapport cyclique faisait un fort appel de courant qui déclanchait une sécurité du hacheur (HALL OVER CURRENT).
La solution pour palier à ce problème est de faire varier la rapport cyclique petit à petit. Dans notre cas, nous avons choisi d'augmenter (ou de diminuer) de 1% par seconde.

* Une fois que nous avons fais les tests nécessaires pour voir si le moteur tournait correctement et que la sécurité du hacheur ne se déclanchait pas, nous avons mis en place la commande ``speed`` permettant de régler le rapport cyclique via la console et non pas directement dans le code.

* Pour finir, nous devions asservir le moteur en courant puis en vitesse.

Pour cela nous avons du faire des test pour récuperer les caractéristique du moteur car les valeurs indiquées sur la plaque n'étaient pas forcément correctes.

Nous pouvons voir sur les images si dessous les valeurs récuperées à l'aide d'un ocsilloscope.

Nous identifions la constante de temps éléctrique (Te) en luis imposant un échelon de tension.
![image](https://user-images.githubusercontent.com/86347317/144016148-a4bf1a62-6fa8-4f73-b89f-2b8c97531224.png)

Ici nous mettons brutallement la courant à zéro pour pouvoir lire la valeur de la constante de temps mécanique (Tm).
![image](https://user-images.githubusercontent.com/86347317/144995396-7464b7b2-ab2b-4714-8bb3-85770913a6bf.png)

Mesure moteur : 
```
R = 6.5 Ω (mesuré à l'Ohmmètre)
Te = 18 ms (Vfinal = 930 mV à peu près)
L = 117 mH

test (U = 48 V, 1.77 A)
Kphi = 0.404 V / rd/s
f = 7*e-5 (Valeur incohérente, elle devrait de l'ordre de grandeur de e-4)
gamma0 = 0,19 Nm
Tm = 716 ms
```

Une fois les caractéristiques trouvées, nous devions récuperer le valeur du courant à l'aide des capteurs à effet Hall. Nous avions à récuperer les valeurs du capteur à l'aide de l'ADC qui écrit dans la mémoire en passant par le DMA (Direct Memory Access). Cette méthode permet de ne pas monopoliser le CPU du microcontroleur.

Cependant, nous avons commencer à coder les fonctions pour récuperer les valeurs, mais nous n'avons pas réussi a faire fonctionner l'interruption du DMA. Donc nous n'étions pas capable de récuperer les bonnes valeurs.

