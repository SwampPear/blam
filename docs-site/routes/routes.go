package routes

import (
    "github.com/gofiber/fiber/v2"
    "github.com/SwampPear/personal-website/handlers"
)

func SetupRoutes(app *fiber.App) {
	app.Get("/", handlers.Index)
	app.Use(handlers.Error404)
}