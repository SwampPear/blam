package main

import (
    "github.com/gofiber/fiber/v2"
	"github.com/gofiber/template/html/v2"
    "github.com/SwampPear/personal-website/routes"
)

func main() {
	engine := html.New("./templates", ".html")

	app := fiber.New(fiber.Config{
		Views: engine,
	})

	app.Static("/", "./static")

	routes.SetupRoutes(app)

    app.Listen(":3000")
}

