# FRED-covid-co
COVID-19 agent-based model in FRED 

## **FRED-COVID-CO**

Este repositorio contiene una versión modificada de **FRED**, un modelo basado en agentes, originalmente diseñado para modelar epidemias de influenza, pero que ha sido adaptado sucesivamente, para modelar la dinámica epidemiológica de varias enfermedades.

La versión presente fue adaptada en el marco del proyecto **Modelo basado en agentes para evaluar estrategias de control de la epidemia COVID-19 en Colombia**, con financiación de la Vicerrectoría de Investigación de la Universidad Nacional de Colombia. En este proyecto, se desarrollaron las herramientas para poder simular la dinámica de infecciones COVID en diversas localidades de Colombia, así como las diferentes medidas farmacológicas y no farmacológicas disponibles para el control de la epidemia.

### Colaboradores

En la elaboración de esta adaptación participaron las siguientes personas:

- Guido España, Notre Dame University 

- Diego Velandia, Universidad Nacional de Colombia

- Hernando Díaz, Universidad Nacional de Colombia

- Nelson Castañeda, Escuela tecnológica Instituto Técnico Central

## FRED A Framework for Reconstructing Epidemiological Dynamics
 **FRED** es un sistema "open source" desarrollado en la  University of Pittsburgh [Public Health Dynamics Laboratory](http://www.phdl.pitt.edu "PHDL website") en colaboración con el [Pittsburgh Supercomputing Center](http://www.psc.edu "PSC website") y la [Carnegie Mellon University School of Computer Science](http://www.cs.cmu.edu "CMU CS website"). 

### Versión
Esta versión ha sido adaptada a partir del sistema **FRED** original para modelar las características de la infección de COVID-19, así como algunas medidas de control implementadas o propuestas en Colombia:

- Testeo PCR de personas contagiadas

- Testeo aleatorio de personas susceptibles o infectados asintomáticos

- Cierre y reapertura parcial de escuelas

- Vacunas con diferentes efectividades


### Notes on compiling (tomado de la versión original)
By default FRED will try to use the clang compiler (newer versions of XCode for Mac will have this). If your compiler is not clang, you should find this section of **FRED/src/Makefile** and comment it out.

```
# comment out if not using clang
CLANG_FLAGS = -mllvm -inline-threshold=1000
```

so that it looks like this:

```
# comment out if not using clang
# CLANG_FLAGS = -mllvm -inline-threshold=1000
```

## Shiny App Interfaz FRED-COVID

Para ejecutar FRED desde un ambiente interactivo, más amigable que la interfaz normal, basada en archivos de datos,  se creó una interfaz gráfica, basada en una **Shiny-App** ejecutable desde **R** o **RStudio**. De esta manera, se facilita simular diferentes alternativas  y variantes, para un modelo determinado. Esta aplicación se puede descargar desde el repositorio <https://github.com/hdodiaz/FRED_covid_shiny.git>.

## Licencia
La licencia del software original requiere que las versiones derivadas se comprometan a respetar los términos establecidos por los autores de la versión inicial. Por lo tanto, si usted desea utlizar esta versión, deberá comprometerse con los términos siguientes:



FRED is released under the BSD 3-Clause ("BSD New" or "BSD Simplified")
license:

Copyright (c) 2010-2012, University of Pittsburgh, John Grefenstette,
Shawn Brown, Roni Rosenfield, Alona Fyshe, David Galloway, Nathan Stone,
Jay DePasse, Anuroop Sriram, and Donald Burke.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

* Neither the name of the University of Pittsburgh nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
