# ⚙️ Architecture de CSigma — Système d’évènements et moteur de traduction intelligente

### Vue d’ensemble

**CSigma** est un framework C++ actuellement en développement actif, conçu pour offrir un environnement **léger, performant et entièrement natif** destiné à la création d’interfaces graphiques modernes.  
Il repose sur deux piliers techniques essentiels :

1. Un **système d’évènements groupé**, inspiré de Win32, offrant un contrôle optimal à l’exécution.  
2. Un **moteur de traduction intelligente**, assisté par IA, capable de changer instantanément la langue de l’interface avec un contrôle précis de la qualité de traduction.

---

## 🧩 1. Système d’évènements (inspiré de Win32, modernisé)

### Concept

CSigma adopte un modèle événementiel basé sur la **distribution directe des messages**, suivant les principes d’efficacité de l’API Win32.  
Chaque élément de l’interface enregistre une ou plusieurs **actions** en réponse aux messages du système (clics, saisies, survols, etc.).

```cpp
CSUIMAN::addAction(ControlID, HandlerFunction, ArgsCount, ...);
```

Exemple de gestionnaire :

```cpp
void onClickExample(CSARGS Args) {
    if (UINT(Args) == WM_LBUTTONUP)
        MessageBoxW(NULL, L"Action exécutée avec succès !", L"CSigma", MB_OK);
}
```

Cette approche offre aux développeurs un **contrôle complet** sur les interactions tout en garantissant une **latence minimale** et un **comportement prévisible**.

---

### Profil de performance

Le répartiteur d’évènements de CSigma fonctionne en **résolution constante**, sans réflexion, ni signaux/slots, ni redirection virtuelle.  
Il offre ainsi des performances **proches du natif**, comparables aux boucles de messages Win32, même pour des interfaces complexes.

| Framework | Modèle d’évènements | Latence moyenne | Finesse du contrôle | Sécurité multithread | Surcharge |
|------------|---------------------|------------------|----------------------|----------------------|-----------|
| **Win32 API** | Callback `WndProc` | ⚡ Très faible | 🔧 Totale | Manuelle | Minimale |
| **Qt** | Système de signaux/slots | 🐢 Élevée | 🟡 Moyenne | Partielle | Modérée |
| **wxWidgets** | Tables d’évènements | ⚙️ Moyenne | 🟡 Moyenne | Partielle | Modérée |
| **ImGui** | Boucle centralisée | ⚡ Très faible | 🟠 Limitée | ✅ Oui | Faible |
| **CSigma** | Callbacks groupés | ⚡ Très faible | 🔧 Totale | ✅ Gérée | Minimale |

**En résumé :**  
CSigma offre la **réactivité directe de Win32** tout en proposant un modèle d’attachement d’évènements plus clair et modulaire, permettant un **contrôle fin à l’exécution** sans les surcouches des frameworks lourds.

---

### Points techniques clés

- **Mappage direct** sur les messages `WM_*` natifs (aucune émulation)  
- **Groupement par section**, facilitant la conception modulaire des interfaces  
- **Interface unifiée (`CSARGS`)** pour tous les types de contrôles  
- **Compatible multithread**, les évènements peuvent être postés sans blocage  
- **Aucune dépendance à la réflexion** ou aux systèmes d’objets dynamiques  

Cette architecture garantit des **temps de réponse déterministes**, même sous forte activité graphique ou multi-tâches.

---

## 🌐 2. Moteur de traduction intelligente (CSLANGMAN)

### Concept

CSigma introduit un **système de traduction en temps réel** allant au-delà de la localisation statique traditionnelle.  
Grâce à `CSLANGMAN`, toute application basée sur CSigma peut changer instantanément de langue **sans rechargement de l’interface** ni redémarrage du programme.

Le moteur est **assisté par IA**, capable de générer dynamiquement des traductions précises via un backend local ou distant.  
Des mécanismes de **contrôle qualité** garantissent la cohérence linguistique et permettent la mise en cache pour un usage hors ligne.

---

### Exemple d’utilisation

```cpp
CSLANGMAN::translateAppStrings();
```

Tous les éléments visibles (titres, boutons, info-bulles, dialogues) sont mis à jour automatiquement, quelle que soit leur hiérarchie.

---

### Comparaison avec d’autres frameworks

| Framework | Système de traduction | Changement dynamique | Intégration IA | Finesse du contrôle | Remarques |
|------------|----------------------|----------------------|----------------|----------------------|------------|
| **Qt** | Fichiers `.ts` + `QTranslator` | ⚠️ Partiel (rechargement requis) | ❌ Aucune | 🟡 Par widget | Rechargement manuel |
| **wxWidgets** | `.mo` via `wxLocale` | ❌ Non | ❌ Aucune | ⚙️ Par application | Fichiers statiques |
| **GTK / gettext** | `.po` / `.mo` | ❌ Non | ❌ Aucune | 🟡 Moyenne | Re-initialisation nécessaire |
| **ImGui / SDL** | Remplacement manuel | ❌ Non | ❌ Aucune | 🔧 Totale (manuelle) | Gestion par le développeur |
| **CSigma** | `CSLANGMAN` en mémoire | ✅ Oui (instantané) | ✅ Intégrée | 🔧 Totale | Traduction contrôlée et dynamique |

---

### Points techniques clés

- **Cartographie multilingue en mémoire** (aucune relecture de fichier)  
- **Couche IA** pour la traduction automatique et adaptative  
- **Propagation instantanée** : tous les contrôles se mettent à jour en temps réel  
- **Validation qualité** des traductions (cohérence, grammaire)  
- **Mécanisme de secours** vers la langue par défaut en cas d’absence de clé  

> En pratique, CSigma peut re-traduire toute son interface en quelques millisecondes, sans altérer la structure visuelle ni le rendu.

---

## 🧠 3. État du développement

CSigma est **en développement actif**.  
L’architecture, le répartiteur d’évènements et le moteur de traduction sont déjà fonctionnels et évoluent vers une modularité accrue et une compatibilité multiplateforme.

Les développements en cours incluent :  
- L’élargissement des contrôles graphiques disponibles  
- L’intégration étendue de backends IA (en ligne et hors ligne)  
- L’amélioration du lien entre `CSSECMAN` et `CSLANGMAN` pour la gestion du contenu dynamique  

Même à ce stade, CSigma démontre une **réactivité en temps réel robuste** et une **approche unique de la traduction intelligente** dans un environnement C++ natif.

---

## 🏁 4. Résumé

| Fonctionnalité | Qt | wxWidgets | ImGui | Win32 | **CSigma** |
|----------------|----|-----------|--------|--------|-------------|
| API C++ native | ✅ | ✅ | ✅ | ✅ | ✅ |
| Runtime léger | ⚠️ | ⚠️ | ✅ | ✅ | ✅ |
| Réactivité évènementielle | 🟡 | 🟡 | ✅ | ✅ | ✅ |
| Traduction dynamique | ⚠️ Partielle | ❌ | ❌ | ❌ | ✅ Instantanée |
| Traduction IA | ❌ | ❌ | ❌ | ❌ | ✅ Intégrée |
| Messagerie thread-safe | 🟡 | 🟡 | ✅ | ⚙️ Manuelle | ✅ Gérée |
| Rafraîchissement temps réel | ⚠️ Limité | ⚠️ | ✅ | ⚙️ | ✅ Complet |
| Développement ouvert | ✅ | ✅ | ✅ | ✅ | 🚧 En cours |

---

### En résumé
> **CSigma** redéfinit le développement d’interfaces C++ natives en combinant un **système d’évènements de niveau Win32** et un **moteur de traduction temps réel assisté par IA**.  
> Il offre des performances déterministes, un contrôle total à l’exécution et la possibilité de localiser instantanément toute application, dans n’importe quelle langue, au sein d’une architecture moderne, modulaire et cohérente.
