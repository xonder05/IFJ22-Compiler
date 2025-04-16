# 🛠️ IFJ22 Compiler

Implementation of a compiler for the imperative programming language **IFJ22** (subset of PHP), created as a team project for the university courses **IFJ (Formal Languages and Compilers)** and **IAL (Algorithms)** at FIT VUT Brno.

## 📚 Project Description

This project involves building a compiler in **C** that reads source code written in the **IFJ22** language and translates it into **IFJcode22**, an intermediate code format. The project scope includes:

- **Lexical Analysis**
- **Syntax Analysis (LL and Precedence Parsing)**
- **Semantic Analysis**
- **Code Generation**
- **Symbol Table Management**
- **Abstract Syntax Tree Construction**

The compiler operates as a console application (filter) that reads input from **stdin**, prints output to **stdout**, and logs all errors to **stderr**.

## ⚙️ Language Overview

- **IFJ22** is a simplified subset of PHP.
- It supports:
  - User-defined functions
  - Variable declarations
  - Control structures
  - Recursion
- **IFJcode22** is a custom intermediate language based on three-address and stack instructions.


## Note
For more details pleas see `dokumentace.pdf` or `zadani.pdf`.

## 🧩 Team Members & Responsibilities

| Name               | Contributions                                              |
|--------------------|------------------------------------------------------------|
| **Ondřej Bahounek** | Lexical analysis, code generation, semantic analysis       |
| **Aleksandr Kasianov** | Top-down syntax analysis (LL), symbol table               |
| **Daniel Onderka**   | Precedence syntax analysis, abstract syntax tree          |
| **Tomáš Prokop**     | Documentation, presentation, code fixes, Makefile         |




## 📝 Course Information

- **IFJ**: Formal Languages and Compilers  
- **IAL**: Algorithms  
- **University**: Brno University of Technology (VUT FIT)  
- **Academic Year**: 2022/2023  
- **Instructors**: Zbyněk Křivka, Radim Kocman, Dominika Klobučníková

---

